/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "syscall_handler_process.h"
#include "console.h"
#include "iso.h"
#include "memorylayout.h" // PROCESS_ENTRY_POINT
#include "permissions_capabilities.h"

#define PROCESS_COPY_CHUNK 3500

int32_t sys_exit(uint32_t code) {
    process_exit((int32_t)code);
    return 0;
}

int32_t sys_yield() {
    process_yield();
    return 0;
}

int32_t sys_run(const char *process_path, const uint32_t permissions_identifier, struct process *parent) {

    // Get the capability
    if (capability_owned_by_process(permissions_identifier, current) != 1) {
        console_printf("Error: could not get permissions capability\n");
        return -1;
    }

    // Load process data
    struct iso_dir *root_dir = iso_dopen("/", 3);
    if (root_dir == 0) {
        console_printf("Error accessing binary directory\n");
        return -1;
    }

    struct iso_file *proc_file = iso_fopen(process_path, root_dir->ata_unit);
    if (proc_file == 0) {
        console_printf("Error accessing binary file\n");
        return -1;
    }

    // store the current number of pages used, so we can see how many the child uses
    int page_count_before_child = parent->number_of_pages_using;

    // Create a new process(page, page)
    struct process *child_proc = process_create(PAGE_SIZE, PAGE_SIZE);

    if (child_proc <= 0) {
        // free the intermediary memory we used
        process_cleanup(child_proc);
        console_printf("Error creating process\n");
        return -1;
    }

    // incorporate the permissions
    struct process_permissions *child_permissions = permissions_from_identifier(permissions_identifier);
    child_proc->permissions = child_permissions;
    child_proc->parent = parent; // store the child's parent

    // Load the code into the proper page
    uint32_t real_addr;
    if (!pagetable_getmap(child_proc->pagetable, PROCESS_ENTRY_POINT, &real_addr)) {
        console_printf("Unable to get physical address of 0x80000000\n");
        // free the intermediary memory we used
        process_cleanup(child_proc);
        return -1;
    }


    // transfer pages used count to child
    int child_pages_used = parent->number_of_pages_using - page_count_before_child;
    child_proc->number_of_pages_using += child_pages_used;
    parent->number_of_pages_using = page_count_before_child;

    // add the max number of pages child could use to parent's usage
    parent->number_of_pages_using += child_proc->permissions->max_number_of_pages;


    // check if we've exceeded the parent's allocation
    if (parent->number_of_pages_using > parent->permissions->max_number_of_pages) {
        console_printf("Error: process %d attempted to create a process %d without available memory: %d > %d\n", parent->pid, child_proc->pid, parent->number_of_pages_using, parent->permissions->max_number_of_pages);
        process_cleanup(child_proc);
        return -1;
    }

    // check if we've exceeded the child's allocation
    if (child_proc->number_of_pages_using > child_proc->permissions->max_number_of_pages) {
        console_printf("Error: child process %d exceeded its limit\n", child_proc->pid);
        process_cleanup(child_proc);
        return -1;
    }

    // get and copy the data, chunks at a time
    uint8_t *process_data = kmalloc(PROCESS_COPY_CHUNK); // intermediary space
    if (process_data == 0) {
        // free the intermediary memory we used
        kfree(process_data);
        console_printf("Error allocating intermediary space\n");
        return -1;
    }

    int amount_copied = 0; // amount copied so far
    uint32_t copy_location = real_addr; // travels with amount copies

    // while there is data to copy
    while (amount_copied < proc_file->data_length) {
        int amount_left = proc_file->data_length - amount_copied; // compute amount left
        int to_be_copied = amount_left < PROCESS_COPY_CHUNK ? amount_left : PROCESS_COPY_CHUNK; // get either chunk or the rest

        int num_read = iso_fread(process_data, to_be_copied, 1, proc_file); // read the chunk
        if (num_read == 0) {
            // free the intermediary memory we used
            kfree(process_data);
            console_printf("Error reading binary data\n");
            return -1;
        }
        memcpy((void *)copy_location, (void *)process_data, to_be_copied); // copy it to process entry point
        amount_copied += to_be_copied;
        copy_location += to_be_copied;
    }

    // free the intermediary memory we used
    kfree(process_data);

    // Push the new process onto the ready list
    add_process_to_ready_queue(child_proc);

    // yield the current process to the new one
    process_yield();
    return 0;
}
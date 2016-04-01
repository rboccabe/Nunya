/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "interrupt.h"
#include "syscall.h"
#include "console.h"
#include "process.h"
#include "fs_sys.h"
#include "window.h"

#define CHECK_PROC_WINDOW() if(current->window == 0) return 1

uint32_t sys_exit(uint32_t code) {
    process_exit(code);
    return 0;
}

uint32_t sys_yield() {
    process_yield();
    return 0;
}

uint32_t sys_testcall(uint32_t code) {
    console_printf("testing: %d\n", code);
    return 0;
}

uint32_t sys_open(uint32_t path_intptr, uint32_t mode_intptr) {
    return fs_sys_open((const char *)path_intptr, (const char *)mode_intptr);
}

uint32_t sys_close(uint32_t fd) {
    fs_sys_close(fd);
    return 0;
}

uint32_t sys_read(uint32_t dest_intptr, uint32_t bytes, uint32_t fd) {
    return fs_sys_read((char *)dest_intptr, bytes, fd);
}

uint32_t sys_write(uint32_t src_intptr, uint32_t bytes, uint32_t fd) {
    return fs_sys_write((const char *)src_intptr, bytes, fd);
}

uint32_t sys_add_fs_allow(uint32_t path_intptr, uint32_t do_allow_below) {
    return fs_sys_add_allowance((const char *)path_intptr,  (bool)do_allow_below);
}

uint32_t sys_remove_fs_allow(uint32_t path_intptr) {
    return fs_sys_remove_allowance((const char *)path_intptr);
}

uint32_t sys_window_create(int x, int y, int width, int height) {
    struct window *win = window_create(x, y, width, height, 0);
    current->window = win;
    return win != 0;
}

int32_t sys_set_border_color(const struct graphics_color *color) {
    CHECK_PROC_WINDOW();
    window_set_border_color(current->window, *color);
    return 0;
}

int32_t sys_draw_line(int x1, int y1, int x2, int y2, const struct graphics_color *color) {
    CHECK_PROC_WINDOW();
    window_draw_line(current->window, x1, y1, x2, y2, *color);
    return 0;
}

int32_t sys_draw_char(int x, int y, char c, const struct graphics_color *fgcolor, const struct graphics_color *bgcolor) {
    CHECK_PROC_WINDOW();
    window_draw_char(current->window, x, y, c, *fgcolor, *bgcolor);
    return 0;
}

int32_t sys_draw_string(int x, int y, const char *str, const struct graphics_color *fgcolor,
    const struct graphics_color *bgcolor) {
    CHECK_PROC_WINDOW();
    window_draw_string(current->window, x, y, str, *fgcolor, *bgcolor);
    return 1;
}

int32_t syscall_handler(uint32_t n, uint32_t a, uint32_t b, uint32_t c,
                        uint32_t d, uint32_t e) {
    switch (n) {
        case SYSCALL_exit:
            return sys_exit(a);
        case SYSCALL_testcall:
            return sys_testcall(a);
        case SYSCALL_yield:
            return sys_yield();
        case SYSCALL_open:
            return sys_open(a, b);
        case SYSCALL_close:
            return sys_close(a);
        case SYSCALL_read:
            return sys_read(a, b, c);
        case SYSCALL_write:
            return sys_write(a, b, c);
        case SYSCALL_add_fs_allow:
            return sys_add_fs_allow(a, b);
        case SYSCALL_remove_fs_allow:
            return sys_remove_fs_allow(a);
        case SYSCALL_window_create:
            return sys_window_create(a, b, c, d);
        case SYSCALL_window_set_border_color:
            return sys_set_border_color((const struct graphics_color *)a);
        case SYSCALL_window_draw_line:
            return sys_draw_line(a, b, c, d, (const struct graphics_color *)e);
        case SYSCALL_window_draw_char:
            return sys_draw_char(a, b, (char)c, (const struct graphics_color *)d, (const struct graphics_color *)e);
        case SYSCALL_window_draw_string:
            return sys_draw_string(a, b, (const char *)c, (const struct graphics_color *)d, (const struct graphics_color *)e);
        default:
            return -1;
    }
}

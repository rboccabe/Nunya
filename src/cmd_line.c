/*
Copyright (C) 2015 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/


#include "console.h"
#include "string.h"
#include "testing.h"
#include "fs_terminal_commands.h"

#define KEYBOARD_BUFFER_SIZE 256

void cmd_line_init() {
    set_cur_path(ROOT);
}

void cmd_line_show_prompt() {
    console_set_fgcolor(255, 100, 100);
    console_printf("%s > ", get_cur_path());
    console_set_fgcolor(255, 255, 255);
}

void cmd_line_attempt(const char *line) {
    char line_copy[KEYBOARD_BUFFER_SIZE];
    strcpy(line_copy, line);
    strip_backspace(line_copy); // Get rid of backspace characters

    char *first_word = strtok(line_copy, " ");
    const char *the_rest_mixed_case = line_copy + (strlen(first_word) + 1);
    char the_rest[strlen(the_rest_mixed_case) + 1];
    to_upper(the_rest, the_rest_mixed_case);

    //Ugly case statement alternative
    if (strcmp("echo", first_word) == 0) {
        cmd_line_echo(the_rest);
    } else if (strcmp("test", first_word) == 0) {
        run_all_tests();
    } else if (strcmp("cd", first_word) == 0) {
        cmd_line_cd(the_rest);
    } else if (strcmp("pwd", first_word) == 0) {
        cmd_line_pwd();
    } else if (strcmp("ls", first_word) == 0) {
        cmd_line_ls(the_rest);
    } else if (strcmp("cat", first_word) == 0) {
        cmd_line_cat(the_rest);
    }
    /*else if () {
     *...
     *}
     */
    else {
        console_printf("%s: command not found\n", first_word);
    }
    memset(line_copy, '\0', KEYBOARD_BUFFER_SIZE);
    return;
}

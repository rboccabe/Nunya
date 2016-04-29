int main();
int _start() {
    main();
}

#include "syscall.h"

int main() {
    create_window(10, 10, 400, 400);
    // ls usr
    // --> jesse, ryan, alex
    // restrict /usr/jesse
    // ls usr
    // --> nothing
    struct graphics_color c = {0,255,0};
    struct graphics_color bg = {0,0,0};

    int fd = open("/3/USR/USERS.TXT", "r");
    debug_print(fd);
    while(1){}
    if (fd < 0) {
        draw_string(10, 150, "Failed to open file.\n", &c, &bg);
        exit(0);
    } else {
        draw_string(10, 150, "Opened file successfully.\n", &c, &bg);
    }

    close(fd);

    return 0;
}

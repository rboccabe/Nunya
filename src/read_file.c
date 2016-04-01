int main();
int start() {
    main();
    while(1) {}
    return 0;
}

#include "syscall.h"

int main() {
    create_window(20, 20, 400, 400);

    //start with root permissions, but only allow secondary ata units
    add_fs_allow("/2/", 1);
    add_fs_allow("/3/", 1);
    remove_fs_allow("/");

    //open file
    int fd = open("/3/GETTYSBU.TXT", "r");

    int bytes_read;
    char buffer[28];

    //perform read
    bytes_read = read(buffer, 27, fd);
    buffer[27] = '\0';
    struct graphics_color fgcolor = {255,0,0};
    struct graphics_color bgcolor = {0,0,0};
    draw_string(0, 0, buffer, &fgcolor, &bgcolor);

    close(fd);

    //Remove allowances
    remove_fs_allow("/3/");
    fd = open("/3/GETTYSBU.TXT", "r");
    draw_string(0, 12, "Access to /3/ removed", &fgcolor, &bgcolor);

    //-2 means no success on open, missing allowance
    
    if (fd < 0) {
        int new_fd = fd * -1;
        draw_string(0, 24, "Error on file open: ", &fgcolor, &bgcolor);
        draw_char(0, 36, new_fd + 48, &fgcolor, &bgcolor);
    }
    return 0;
}

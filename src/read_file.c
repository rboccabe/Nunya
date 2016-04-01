int main();
int start() {
    main();
    while(1) {}
    return 0;
}

#include "syscall.h"

int main() {
    create_window(20, 20, 400, 600);

    //start with root permissions, but only allow secondary ata units
    add_fs_allow("/2/", 1);
    add_fs_allow("/3/", 1);
    remove_fs_allow("/");

    //open file
    int fd = open("/3/GETTYSBU.TXT", "r");

    struct graphics_color fgcolor = {255,0,0};
    struct graphics_color bgcolor = {0,0,0};
    int bytes_read;
    int cum_bread = 0;
    char buffer[49];
    int reads = 0;
    //perform read
    while (cum_bread < 1440) {
        bytes_read = read(buffer, 48, fd);
        cum_bread += bytes_read;
        buffer[bytes_read] = '\0';
        draw_string(0, reads * 12, buffer, &fgcolor, &bgcolor);
        reads++;
    }
    close(fd);

    //Remove allowances
    remove_fs_allow("/3/");
    fd = open("/3/GETTYSBU.TXT", "r");
    draw_string(0, 12*reads, "Access to /3/ removed", &fgcolor, &bgcolor);

    //-2 means no success on open, missing allowance
    
    if (fd < 0) {
        int new_fd = fd * -1;
        draw_string(0, 12 * reads + 12, "Error on file open: ", &fgcolor, &bgcolor);
        draw_char(0, 12 * reads + 24, new_fd + 48, &fgcolor, &bgcolor);
    }
    return 0;
}

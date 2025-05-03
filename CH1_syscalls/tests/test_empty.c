#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int fd;
    char* buf;

    fd = open(argv[1], O_RDWR);

    write(fd, "", 0);
    read(fd, buf, 0);

    return errno;
}

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int my_read(int fd, char* buff, unsigned buff_len);

int my_write(int fd, char* buff, unsigned buff_len);

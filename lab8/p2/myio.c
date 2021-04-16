#include "myio.h"

int my_read(int fd, char* buff, unsigned buff_len)
{
    off_t offset = lseek(fd , 0, SEEK_CUR);
    struct stat sb;

    if(fstat(fd, &sb) == -1)
    {
        printf("fstat prob\n");
    }
    unsigned file_len = sb.st_size;
    unsigned read_len = 0;
    if( file_len <= offset + buff_len )
    {
        read_len = file_len - offset;
    }
    else
        read_len = buff_len;
    
    long page_size = sysconf(_SC_PAGE_SIZE);
    unsigned pa_offset = offset & ~(page_size - 1);
    unsigned pa_len = read_len + offset - pa_offset;
    unsigned read_offset = offset - pa_offset;    

    char* map_addr = mmap(NULL,
                            pa_len,
                            PROT_READ,
                            MAP_SHARED | MAP_NORESERVE,
                            fd,
                            pa_offset);

    if( map_addr == MAP_FAILED)
    {
        printf("mmap prob\n");
        exit(1);
    }

    unsigned i;
    for(i=0; i < read_len ; ++i)
    {
        buff[i] = *(map_addr + i + read_offset);
    }
    
    lseek(fd, read_len, SEEK_CUR);
    munmap(map_addr, pa_len);
    return read_len;
}

int my_write(int fd, char* buff, unsigned buff_len)
{
    off_t offset = lseek(fd , 0, SEEK_CUR);
    unsigned write_len = buff_len;
    
    long page_size = sysconf(_SC_PAGE_SIZE);
    unsigned pa_offset = offset & ~(page_size - 1);
    unsigned write_offset = offset - pa_offset;    
    ftruncate(fd, offset + write_len);
//    printf("buff: %s|b_len: %d|off: %ld|trlen: %ld\n",buff,buff_len, offset, offset + write_len);
    char* map_addr = mmap(NULL,
                            write_len,
                            PROT_WRITE | PROT_READ,
                            MAP_SHARED,
                            fd,
                            0);

    if( map_addr == MAP_FAILED)
    {
        printf("mmap prob\n");
        exit(1);
    }

    unsigned i;
    for(i=0; i < write_len ; ++i)
    {
        *(map_addr + i + write_offset) = buff[i];
        printf("Writing %c\n", buff[i]);
    }
    
    msync(map_addr, write_len, MS_SYNC );
    lseek(fd, write_len, SEEK_CUR);
    munmap(map_addr, write_len);
    return write_len;
}

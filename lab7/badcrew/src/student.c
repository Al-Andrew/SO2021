#include "student.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>


void print_student(const Student* stud)
{
    printf("Id: %-5d|An %d|Nume %-50s|Inmatriculat %d|Buget %d|\n",
            stud->id, stud->an, stud->nume, stud->st_inmatriculare, stud->la_buget);
}

void list_students(char file[])
{
    /*Open the file descriptor*/
    int fd = open(file, O_RDONLY);
    /*A student struct to read into*/
    Student stud;
    int bytes = 0;
    /*Reading loop*/
    while( true )
    {
        bytes = read(fd, &stud, sizeof(Student));
        if( bytes == 0 )
            break;
        if( bytes == -1 )
        {
            perror("Error while reading from file");
            exit(FILE_READ_ERR);
        }
        print_student(&stud);
    }
    /*Cleanup*/
    close(fd);
}

void add_student(char file[], char nume[], int an, bool inmat, bool buget){
    /*Open the file descriptor*/
    int fd = open(file, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    /*Set a lock on the whole file*/
    struct flock lock;
    memset(&lock, 0, sizeof(struct flock));

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd, F_SETLKW, &lock);

    /*Find an id for the student*/
    int id = 0;
    { /*We don't need bytes/stud to spil in the function scope*/
    int bytes;
    Student stud;
    while(true)
    {
        bytes = read(fd, &stud, sizeof(Student));
        if( bytes == 0 )
            break;
        if( bytes == -1 )
        {
            perror("Error while reading from file");
            exit(FILE_READ_ERR);
        }
        if( stud.id > id )
            id = stud.id;
    }
    } /*End of whiel extended scope*/
    ++id; /*We need the next id*/
    /*Construct a student struct*/
    Student new_stud = {
        .id = id,
        .an = an,
        .st_inmatriculare = inmat,
        .la_buget = buget
    };
    strcpy(new_stud.nume, nume); /*Vrem o copie a numelui nu doar sa schimbam pointerii*/

    /*Write the struct to the file*/
    write(fd, &new_stud, sizeof(Student));

    /*Unlock?*/
    /*Close file descriptor*/
    close(fd);
}


void src_student_name(char file[], char nume[])
{
     /*Open the file descriptor*/
    int fd = open(file, O_RDONLY);
    /*A student struct to read into*/
    Student stud;
    int bytes = 0;
    bool found;
    /*Reading loop*/
    while( true )
    {
        bytes = read(fd, &stud, sizeof(Student));
        if( bytes == 0 )
            break;
        if( bytes == -1 )
        {
            perror("Error while reading from file");
            exit(FILE_READ_ERR);
        }
        if( strcmp(stud.nume, nume) == 0 )
        {
            print_student(&stud);
            found = true;
        }
    }
    if( !found )
        printf("Student not found\n");
    /*Cleanup*/
    close(fd);
}

void src_student_id(char file[], int id)
{
     /*Open the file descriptor*/
    int fd = open(file, O_RDONLY);
    /*A student struct to read into*/
    Student stud;
    int bytes = 0;
    bool found;
    /*Reading loop*/
    while( true )
    {
        bytes = read(fd, &stud, sizeof(Student));
        if( bytes == 0 )
            break;
        if( bytes == -1 )
        {
            perror("Error while reading from file");
            exit(FILE_READ_ERR);
        }
        if( stud.id == id )
        {
            print_student(&stud);
            found = true;
            break;
        }
    }
    if( !found )
        printf("Student not found\n");
    /*Cleanup*/
    close(fd);
}

void mod_student(char file[], int id, int an, bool inmat, bool buget){
    /*Open the file descriptor*/
    int fd = open(file, O_RDWR);
    /*Do we need to lock? Can you lock past the end?*/
    /*Find an id for the student*/
    bool found = false;
    int bytes;
    off_t wr_offset;
    Student stud;
    while(true)
    {
        bytes = read(fd, &stud, sizeof(Student));
        if( bytes == 0 )
            break;
        if( bytes == -1 )
        {
            perror("Error while reading from file");
            exit(FILE_READ_ERR);
        }
        if( stud.id == id )
        {
            wr_offset = lseek(fd, 0, SEEK_CUR);
            found = true;
            break;
        }
    }
    /*Maybe we don't have that student*/
    if(!found)
    {
        perror("No such student\n");
        return;
    }
    
    /*Making a lock*/
    struct flock lock;
    memset(&lock, 0, sizeof(struct flock));

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd, F_SETLKW, &lock);

    /*Read again to check if the value changed while locking */
    Student repeat_student;
    lseek(fd, wr_offset - sizeof(Student), SEEK_SET);
    read(fd, &repeat_student, sizeof(Student));
    if(!(strcmp(repeat_student.nume, stud.nume) == 0 &&
        repeat_student.id == stud.id     &&
        repeat_student.an == stud.an     &&
        repeat_student.st_inmatriculare == stud.st_inmatriculare &&
        repeat_student.la_buget == stud.la_buget 
        )){
        
        perror("Failed modification\n");
        return;
    }

    /*Make the modificaitons*/
    stud.an = an;
    stud.st_inmatriculare = inmat;
    stud.la_buget = buget;
    
    /*Lseek back in the file so we overwrite the previous data*/
    lseek(fd, wr_offset - sizeof(Student), SEEK_SET);
    /*Write the struct to the file*/
    write(fd, &stud, sizeof(Student));

    /*Unlock?*/
    /*Close file descriptor*/
    close(fd);
}

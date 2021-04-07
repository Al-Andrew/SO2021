#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include "student.h"

void parse_file(char file[], char db[])
{
    char line_buf[255];
    FILE* fptr;
    fptr = fopen(file, "r");
    while(!feof(fptr))
    {
        fscanf(fptr, "%[^\n] ", line_buf);
        char cmd[32];
        char nume[50];
        int an, id, inmat, buget;
        sscanf(line_buf, "%s", cmd);
        if( strcmp(cmd, "adauga") == 0 ) {
            sscanf(line_buf, "%s %s %d %d %d", cmd, nume, &an, &inmat, &buget);
            add_student(db, nume, an, inmat, buget);
        }
        else if( strcmp(cmd, "modifica") == 0) {
            sscanf(line_buf, "%s %d %d %d %d", cmd, &id, &an, &inmat, &buget);
            mod_student(db, id, an, inmat, buget);
        }
        else if( strcmp(cmd, "cauta-id") == 0) {
            sscanf(line_buf, "%s %d ", cmd, &id);
            src_student_id(db, id);
        }
        else if( strcmp(cmd, "cauta-nume") == 0) {
            sscanf(line_buf, "%s %s", cmd, nume);
            src_student_name(db, nume);
        }
        else if( strcmp(cmd, "listeaza") == 0)
            list_students(db);
    }
    fclose(fptr);
}

void print_help(){
    printf("Usage:\n"\
           "badcrew [commands] [database]\n"\
           "If [database] doesn't exist it will be created\n");
}

int main(int argc, char** argv) {
    if( argc != 3 )
        print_help();
    else
        parse_file(argv[1], argv[2]);
}

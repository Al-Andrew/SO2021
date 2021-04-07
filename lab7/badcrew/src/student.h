#ifndef HG_STUDENT
#define HG_STUDENT
#include <stdbool.h>


#define FILE_READ_ERR 1
#define FILE_OPEN_ERR 2
#define FILE_WRITE_ERR 3
#define FILE_EXIT_ERR 4

typedef struct Student {
    int id;
    int an;
    char nume[50];
    bool st_inmatriculare;
    bool la_buget;
} Student;

void add_student(char file[], char name[], int an, bool inmat, bool buget);
void src_student_name(char file[], char nume[]);
void src_student_id(char file[], int id);
void mod_student(char file[], int id, int an, bool inmat, bool buget);
void list_students(char file[]);

#endif //HG_STUDENT

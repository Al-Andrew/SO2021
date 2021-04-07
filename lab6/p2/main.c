#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#define E_LINE_TOO_LONG 1
#define E_FD 2


int readln(int fd, char* buf, int buf_len)
{
    int bytes = 0;
    bytes = read( fd, buf, buf_len);
    if( bytes == 0  || bytes == -1)
    {
	return E_FD;
    }
    int i = 0;
    for(i=0; i < buf_len; i++)
    {
	if( buf[i] == '\n' )
	    break;
    }
    if( i == buf_len)
    	return E_LINE_TOO_LONG;
    
    buf[i] = '\0';
    lseek(fd, (-1)*(bytes - i), SEEK_CUR);
    return bytes-i;
}

void print_help()
{
    printf(
    "ncut -- help\n"\
    "Cuts the [files] into fields or columns and displys the according to [options]\n"\
    "Usage :\n"\
    "ncut [options] [files]\n"\
    "[options] can be:\n"\
    "	-f  specifies wich fields should be printed ( needs a -d )\n"\
    "	-d  sets the field delimiter ( defalt is ' ' )\n"\
    "	-b  specifies which bytes of a line should be printed ( conflicts with -d and -f )\n"
    "Examples:\n"\
    "ncut -d : -f 1,2 file.txt    | This will print the first and second fields separated by :\n"
    "ncut -f 1-4 file.txt         | This will print the fields from one to four separated by ' '\n"\
    "ncut -b 3 file.txt           | This will print the 3rd byte of each line\n"\
    "ncut -b -3 file.txt          | This will print the first 3 bytes of each line\n"\
    "ncut -b 3- file.txt          | This will print from the third to the last bytes of each line\n"\
    "ncut -b 3-7 file.txt         | This will print from the third to the seventh byte of each line\n"\
    );
}

#define INPUT_BYTE 1
#define INPUT_FIELD 2
#define INPUT_HELP 3
#define INPUT_EXIT 4

char delim = ' '; /* The default delimiter is a space */
/*These are set if we use ranges*/
int start = -1;
int end = -1; /* if -1 prints to the last byte/field */ 
/*This is set if we use a list | Thus we can have up to 64 terms*/
unsigned long long PRINT_LIST = 0;
int highest_field = -1;
#define POS(x) 1<<x


int first_file = 0; /* The position of the first file in argv. We assume all the other fis come straight after */


int parse_list(char *input)
{
    char* p;
    p = strtok(input, ",");
    while(p != NULL)
    {
	int pos;
	pos = atoi(p);
	if( p == 0 )
	{
	    printf("Error parsing input list\s");
            return INPUT_HELP;
	}
	
	PRINT_LIST |= POS(pos);
	p = strtok(NULL, ",");
    }
    return 0;
}

#define NR_MAX_LEN 4
int parse_range(char *input, char* line)
{
    char st[NR_MAX_LEN]; 
    strncpy(st, input, line - input);
    start = atoi(st);
    char ed[NR_MAX_LEN];
    strcpy(ed, line+1);
    end = atoi(ed);
    return 0;
}

int parse_value(char* input)
{
     char *line = strchr(input, '-');
     if(line == NULL)
         return parse_list(input);
     return parse_range(input, line);
}

int handle_input(int argc, char** argv)
{
    if(argc <= 3)
	return INPUT_HELP;
    int i;
    int success = 0;
    int error = 0;
    for(i = 1; i < argc ; i++)
    {
	if( strcmp(argv[i], "-d") == 0 )
	    delim = argv[++i][0];
	else if( strcmp(argv[i], "-f") == 0 )
    	{
	    error = parse_value(argv[++i]);
	    success = INPUT_FIELD;
	}
	else if( strcmp(argv[i], "-b") == 0 )
	{
	    error = parse_value(argv[++i]);
	    success = INPUT_BYTE;
	}
	else
	{
	    first_file = i;
	    break;
	}
    }
    return error == 0? success : error ;
}
#define MAX_LINE_LEN 255

void print_if_ok_field(char* field, int field_n)
{
    if(  PRINT_LIST & POS(field_n) != 0)
    {
	printf("%s%c", field, delim);
	return;
    }
    if( start != -1 && end != -1)
	if( (field_n >= start && field_n <= end) &&
	    (field_n >= start && end == 0) &&
	    (field_n <= end && start == 0) &&
	    (start == 0 && end == 0))
	printf("%s%c", field, delim);
}


void cut_fields(int argc, char** argv)
{
    int file_n = first_file;
    while( file_n < argc)
    {
	int fd = open( argv[file_n], O_RDONLY );
	if( fd == 0 )
	{
	    printf("Error reading from file");
	    exit(2);
	}
	char buff[MAX_LINE_LEN];
	while(readln(fd, buff, MAX_LINE_LEN))
	{
	char *p = 0;
	int field_n = 1;
	p = strtok(buff, delim);
	while( p != NULL )
	{
	    print_if_ok_field(p, field_n);
	    p = strtok(NULL, delim);
	}
	}
	file_n++;
    }
}

int main(int argc, char** argv)
{
    switch(handle_input(argc, argv))
    {
	case INPUT_HELP: print_help(); break;
	case INPUT_EXIT: return 0; break;
	case INPUT_FIELD: cut_fields(argc, argv); break;
	case INPUT_BYTE: return 0; break;
    }
    return 0;
}

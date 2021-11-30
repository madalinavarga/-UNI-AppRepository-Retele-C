
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1
#define PORT 4444
#define config_file "config.txt"
#define apps_file "apps.txt"
using namespace std;

void writeInFile(char *output_string)
{
    FILE *file_fd = fopen(apps_file, "a");
    strcat(output_string, "\n");
    fprintf(file_fd, output_string);
    fclose(file_fd);
}

int main()
{
    char strring[] = "asdsad";
    char *ptr = strring;
    writeInFile(ptr);
}
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

char *getInputCommand(char *inputString) //login: madalina parola
{
    char *subString;
    subString = strrchr(inputString, ':') + 1;
    subString[strlen(subString) - 1] = '\0';
    return subString; // return madalina parola
}
char *getUserName(char *subString)
{
    char delim[] = " ";
    char *ptr_name = strtok(subString, delim);
    return ptr_name;
}

int main(int argc, char *argv[])
{
    printf("[client]Introduceti un nume: ");
    fflush(stdout);
    char nume[100];
    read(0, nume, 100); //change
    char *rezultat = getInputCommand(nume);
    char *ptr = getUserName(rezultat);
    printf("<%s>\n", ptr);
    char numme[20];
    strcpy(numme, ptr);
    printf("<%s>", numme);
}
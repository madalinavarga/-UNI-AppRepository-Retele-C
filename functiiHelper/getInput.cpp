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

int main(int argc, char *argv[])
{
    printf("[client]Introduceti un nume: ");
    fflush(stdout);
    char nume[100];
    read(0, nume, 100); //change
    char *ceva = nume;
    char *rezultat = getInputCommand(ceva);
    printf("<%s>\n", rezultat);
}
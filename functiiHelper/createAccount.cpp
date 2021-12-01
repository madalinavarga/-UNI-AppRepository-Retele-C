
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

char *getUserName(char *subString)
{
    char delim[] = " ";
    char *ptr_name = strtok(subString, delim);
    return ptr_name;
}
int checkExistingUserNameOnly(char *nameToFind)
{
    FILE *configFd = fopen(config_file, "r");

    char wordAux[100];
    char nume[50];
    int count = 0;

    while (1)
    {
        if (fscanf(configFd, "%s", wordAux) == EOF)
            break;
        else
        {
            char *ptr_name = getUserName(wordAux);

            if (strcmp(ptr_name, nameToFind) == 0)
            {
                count++;
                break;
            }
        }
    }
    fclose(configFd);
    return count;
}

int main()
{
    char nume[] = "narcis";

    int found = checkExistingUserNameOnly(nume);
    printf("exista nume?: %d\n", found);
}
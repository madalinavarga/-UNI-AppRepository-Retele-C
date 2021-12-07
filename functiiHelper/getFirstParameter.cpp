
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
#include <ctype.h>
#include <list>

#define FALSE 0
#define TRUE 1
#define PORT 4449
char config_file[] = "config.txt";
char apps_file[] = "apps.txt";
using namespace std;
int id = 0;
char userName[50];
char *getFirstParameter(char *givenString)
{

    int len = strlen(givenString);
    char copy_subString[len];

    strcpy(copy_subString, givenString);
    char delim[] = " ";
    char *ptr_name = strtok(copy_subString, delim);
    char *copy_pointer = (char *)malloc(strlen(ptr_name));
    strcpy(copy_pointer, ptr_name);

    return copy_pointer;
}
char *getSecondParameter(char *inputString)
{

    char *subString;
    subString = strrchr(inputString, ' ') + 1;
    return subString;
}

class AppDetails
{
public:
    int id = 0;
    char *owner;
    char *name;
    char *about;
    char *author;
    char *websiteLink;
    char *systemRequirements;
    char *price;
    char *ramMemory;
    char *version;
    char *otherDetails;

    AppDetails(char *owner);
    AppDetails(){};
    void setFromJsonFile(char *fileName);
    void setField(char field[], char value[]);
    void setFromCsvLine(char *line);
    char *toString();
};

int main()
{
    char fisier[] = "1234 filename.txt";
    char *a = getFirstParameter(fisier);
    printf("in main 1 id: <%s>\n", a);
    char *b = getSecondParameter(fisier);
    printf("in main 2 nume: %s\n", b);
}
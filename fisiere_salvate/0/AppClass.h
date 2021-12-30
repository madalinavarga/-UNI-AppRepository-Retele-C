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
#define PORT 4447
char *readFile(char *file);
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

char *AppDetails::toString()
{

    char *finalString = (char *)malloc(1000); //change it
    sprintf(finalString, "%d; %s; %s; %s; %s; %s; %s; %s; %s; %s; %s;", this->id, this->owner, this->name, this->about, this->author, this->websiteLink, this->systemRequirements, this->price, this->ramMemory, this->version, this->otherDetails);
    return finalString;
}

AppDetails::AppDetails(char *owner)
{
    this->owner = (char *)malloc(strlen(owner) + 1);
    strcpy(this->owner, owner);
    this->id = id;
    id++;
}

void AppDetails::setFromJsonFile(char *fileName)
{
    char *contents = readFile(fileName);
    char delim[] = "{},:\" \t\n";

    char *field = strtok(contents, delim);
    char *value = strtok(NULL, delim);
    while (field)
    {
        setField(field, value); // daca setfield returneaza false => field nerecunoscut => return false
        field = strtok(NULL, delim);
        value = strtok(NULL, delim);
        printf("\n %s %s ", field, value);
    }
}

void AppDetails::setField(char field[], char value[])
{

    int i = 0;
    printf("Intru aici de %d ori\n", i++);
    if (strcmp(field, "owner") == 0)
    {
        this->owner = (char *)malloc(strlen(value) + 1);
        strcpy(this->owner, value);
    }
    else if (strcmp(field, "name") == 0)
    {
        this->name = (char *)malloc(strlen(value) + 1);
        strcpy(this->name, value);
    }
    else if (strcmp(field, "author") == 0)
    {
        this->author = (char *)malloc(strlen(value) + 1);
        strcpy(this->author, value);
    }
    else if (strcmp(field, "about") == 0)
    {
        this->about = (char *)malloc(strlen(value) + 1);
        strcpy(this->about, value);
    }
    else if (strcmp(field, "websiteLink") == 0)
    {
        this->websiteLink = (char *)malloc(strlen(value) + 1);
        strcpy(this->websiteLink, value);
    }
    else if (strcmp(field, "systemRequirements") == 0)
    {
        this->systemRequirements = (char *)malloc(strlen(value) + 1);
        strcpy(this->systemRequirements, value);
    }
    else if (strcmp(field, "price") == 0)
    {
        this->price = (char *)malloc(strlen(value) + 1);
        strcpy(this->price, value);
    }
    else if (strcmp(field, "ramMemory") == 0)
    {
        this->ramMemory = (char *)malloc(strlen(value) + 1);
        strcpy(this->ramMemory, value);
    }
    else if (strcmp(field, "version") == 0)
    {
        this->version = (char *)malloc(strlen(value) + 1);
        strcpy(this->version, value);
        printf("intru in setare\n");
    }
    else if (strcmp(field, "otherDetails") == 0)
    {
        this->otherDetails = (char *)malloc(strlen(value) + 1);
        strcpy(this->otherDetails, value);
        printf("intru in setare\n");
    }
    // else return false
}

void AppDetails::setFromCsvLine(char *line)
{

    char delim_field[] = " ;";
    char *field = strtok(line, delim_field);
    this->id = atoi(field);

    field = strtok(NULL, delim_field);
    this->owner = (char *)malloc(strlen(field) + 1);
    strcpy(this->owner, field);

    field = strtok(NULL, delim_field);
    this->name = (char *)malloc(strlen(field) + 1);
    strcpy(this->name, field);

    field = strtok(NULL, delim_field);
    this->about = (char *)malloc(strlen(field) + 1);
    strcpy(this->about, field);

    field = strtok(NULL, delim_field);
    this->author = (char *)malloc(strlen(field) + 1);
    strcpy(this->author, field);

    field = strtok(NULL, delim_field);
    this->websiteLink = (char *)malloc(strlen(field) + 1);
    strcpy(this->websiteLink, field);

    field = strtok(NULL, delim_field);
    this->systemRequirements = (char *)malloc(strlen(field) + 1);
    strcpy(this->systemRequirements, field);

    field = strtok(NULL, delim_field);
    this->price = (char *)malloc(strlen(field) + 1);
    strcpy(this->price, field);

    field = strtok(NULL, delim_field);
    this->ramMemory = (char *)malloc(strlen(field) + 1);
    strcpy(this->ramMemory, field);

    field = strtok(NULL, delim_field);
    this->version = (char *)malloc(strlen(field) + 1);
    strcpy(this->version, field);

    field = strtok(NULL, delim_field);
    this->otherDetails = (char *)malloc(strlen(field) + 1);
    strcpy(this->otherDetails, field);
}
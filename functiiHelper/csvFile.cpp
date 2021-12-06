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
#include <list>
using namespace std;
#define file_f "test.txt"

char filename[] = "test.txt";

char *readFile(char *file);

int id = 0;
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
    void setFromCsvLine(char *line);
    char *toString();
};

int main(int argc, char *argv[])
{
    char *contents = readFile(filename); // all content
    list<AppDetails> listOfApps;
    char delim_apps[] = "\n\r";
    char *appDetails = strtok_r(contents, delim_apps, &contents);
    while (appDetails != nullptr)
    {
        AppDetails app;

        app.setFromCsvLine(appDetails);
        listOfApps.push_back(app);
        appDetails = strtok_r(NULL, delim_apps, &contents);
    }

    for (auto x = listOfApps.begin(); x != listOfApps.end(); x++)
    {
        printf("%s\n", x->toString());
    }
}

AppDetails::AppDetails(char *owner)
{
    this->owner = (char *)malloc(strlen(owner) + 1);
    strcpy(this->owner, owner);
    this->id = id;
    id++;
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

char *AppDetails::toString()
{

    char *finalString = (char *)malloc(1000); //change it
    sprintf(finalString, "%d; %s; %s; %s; %s; %s; %s; %s; %s; %s; %s;", this->id, this->owner, this->name, this->about, this->author, this->websiteLink, this->systemRequirements, this->price, this->ramMemory, this->version, this->otherDetails);
    return finalString;
}

char *readFile(char *file)
{
    FILE *file_fd = fopen(file, "r");
    // interesant
    fseek(file_fd, 0, SEEK_END); //merg la final de fisier
    long fsize = ftell(file_fd); //iau pozitia
    fseek(file_fd, 0, SEEK_SET); //merg la inceput de fisier

    char *fileContent = (char *)malloc(fsize + 1);
    fread(fileContent, 1, fsize, file_fd);
    fclose(file_fd);

    fileContent[fsize] = 0;

    return fileContent;
}
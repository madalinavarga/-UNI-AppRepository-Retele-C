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

char *readFile(char *file);

int id = 0;
class AppDetails
{
protected:
    int id = 0;
    char *price;
    char *ramMemory;
    char *owner;

public:
    AppDetails(char *owner);
    void setFromFile(char *fileName);
    void setField(char field[], char value[]);
    char *toString();
};

int main(int argc, char *argv[])
{
    // astea le ai in program
    char fileName[] = "test.json";
    char name[] = "madalina";
    char *mada = name;
    // list<AppDetails> listOfApps;

    // creezi aplicatie noua
    AppDetails app(mada);
    app.setFromFile(fileName); // daca retuneaza false => bad input
    printf("%s", app.toString());

    // adaugi aplicatie in lista
    // listOfApps.push_back(app);

    // iterez prin lista
    // for (auto x = listOfApps.begin(); x != listOfApps.end(); x++)
    // {
    //     if (strcmp(x->owner, "madalina") == 0)
    //         printf("%s", x->toString());
    // }

    // adauga la final de fisier clasa
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

AppDetails::AppDetails(char *owner)
{
    this->owner = (char *)malloc(strlen(owner) + 1);
    strcpy(this->owner, owner);
    this->id = id;
    id++;
}

void AppDetails::setFromFile(char *fileName)
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
    }
}

void AppDetails::setField(char field[], char value[])
{
    if (strcmp(field, "price") == 0)
    {
        this->price = (char *)malloc(strlen(value) + 1);
        strcpy(this->price, value);
        //printf("%s\n", this->price);
    }
    else if (strcmp(field, "ramMemory") == 0)
    {
        this->ramMemory = (char *)malloc(strlen(value) + 1);
        strcpy(this->ramMemory, value);
        // printf("%s\n", this->ramMemory);
    }
    // else return false
}

char *AppDetails::toString()
{
    char *finalString = (char *)malloc(1000);
    sprintf(finalString, "%s %s", this->price, this->ramMemory); //, this->version, this->otherDetails);
    return finalString;
}
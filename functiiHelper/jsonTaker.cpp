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
public:
    int id = 0;
    char price[100] = "-";
    char ramMemory[100] = "-";
    char owner[100];

    AppDetails(char *owner)
    {
        strcpy(this->owner, owner);
        this->id = id;
        id++;
    }

    void setFromFile(char *fileName)
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

    void setField(char field[], char value[])
    {
        if (strcmp(field, "price") == 0)
        {
            strcpy(this->price, value);
        }
        else if (strcmp(field, "ramMemory") == 0)
        {
            strcpy(this->ramMemory, value);
        }
        // else return false
    }

    char *toString()
    {
        char *finalString = (char *)malloc(1000);
        sprintf(finalString, "%s %s", this->price, this->ramMemory); //, this->version, this->otherDetails);
        return finalString;
    }
};

int main(int argc, char *argv[])
{
    // astea le ai in program
    char fileName[] = "test.json";
    // list<AppDetails> listOfApps;

    // creezi aplicatie noua
    AppDetails app("madalina");
    app.setFromFile(fileName); // daca retuneaza false => bad input

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
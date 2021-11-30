
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
    void setFromFile(char *fileName);
    void setField(char field[], char value[]);
    char *toString();
};

void readFromSocket(char *buff, int fd);
void writeInSocket(char buffer[], int fd);
void handle_child(int client_fd, char *msg);
char *getInputCommand(char *);
int checkExistingUser(char *);
char *readFile(char *file);
void writeInFile(char *output_string);

int main(int argc, char *argv[])
{

    struct sockaddr_in server;
    struct sockaddr_in clientStruct;
    int sd;
    char msg[100];

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    bzero(&server, sizeof(server));
    bzero(&clientStruct, sizeof(clientStruct));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    if (listen(sd, 5) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }

    while (1)
    {

        int client;
        socklen_t length; //am modificat
        length = sizeof(clientStruct);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&clientStruct, &length); // returneaza fd pt comunicare cu client
        if (client < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }
        else
            printf("Am acceptat client\n");
        int pid_general;
        if ((pid_general = fork()) == -1)
        {
            perror("[server]Eroare la fork().\n");
        }
        else if (pid_general > 0)
        {
            //parinte
            close(client); // il are fiul
            while (waitpid(-1, NULL, WNOHANG))
                ;
            continue;
        }
        else if (pid_general == 0)
        {
            //copil
            close(sd);
            handle_child(client, msg);
        }
    }
}

void handle_child(int client_fd, char *msg)
{
    int isLogged = FALSE;
    while (1)
    {
        readFromSocket(msg, client_fd);
        printf("\nmesaj:%s", msg);
        if (strstr(msg, "quit"))
        {
            printf("[client]Mesajul primit este: %s. byee client\n", msg);
            writeInSocket(msg, client_fd);
            close(client_fd);
            exit(0);
        }
        else if (strstr(msg, "login : "))
        {
            //search in a file for username and see the password if is ok ->login else -> wrong
            char *userAccountDetails = getInputCommand(msg);
            int found = checkExistingUser(userAccountDetails);
            if (found != 0)
            {
                isLogged = TRUE;
                //scriu ceva la server?
                //cod
                strcpy(msg, "Logged in");
                writeInSocket(msg, client_fd);
            }
            else
            {
                strcpy(msg, "User does not exist");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "addNewApp: "))
        {
            char *filePath = getInputCommand(msg);
            // functie sa citeasca json + sa desparta
        }
        else if (strstr(msg, "newUser: "))
        {
            char *filePath = getInputCommand(msg);
            //de luat detalii +
        }
        else if (strstr(msg, "seeMore: "))
        {
        }
        else if (strstr(msg, "update: ")) //nume utilizator si nume aplciatie
        {
            // verifici daca e logat
            // iau nume utilizator => user
            // iau id aplicatie => id

            //verific daca utilizator==owner
            // gasit = false
            // for (aplicatie in aplicatii)
            // if (aplicatie->name == id && aplicatie->owner == owner )
            // {
            // update + gasit = true
            // }

            // daca gasit e false zici ca nu ai gasit, daca nu, zici ca a fost updatat
        }
        else
        {
            //default
        }
    }
}
char *getInputCommand(char *inputString) //login: madalina parola
{
    char *subString;
    subString = strrchr(inputString, ':') + 1;
    subString[strlen(subString) - 1] = '\0';
    return subString; // return madalina parola
}

int checkExistingUser(char *wordToFind)
{
    FILE *configFd = fopen(config_file, "r");
    char wordAux[100];
    int count = 0;

    while (1)
    {
        if (fscanf(configFd, "%s", wordAux) == EOF)
            break;

        if (strcmp(wordAux, wordToFind) == 0)
        {
            count++;
            break;
        }
    }
    fclose(configFd);
    return count;
}
void readFromSocket(char *buff, int fd)
{
    bzero(buff, 100);
    if (read(fd, buff, 100) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        //return errno;
    }
}

void writeInSocket(char buffer[], int fd)
{

    if (write(fd, buffer, 100) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
        //return errno;
    }
}
char *readFile(char *file)
{
    FILE *file_fd = fopen(file, "r");

    fseek(file_fd, 0, SEEK_END);     //merg la final de fisier
    long file_size = ftell(file_fd); //iau pozitia
    fseek(file_fd, 0, SEEK_SET);     //merg la inceput de fisier

    char *fileContent = (char *)malloc(file_size + 1);
    fread(fileContent, 1, file_size, file_fd);
    fclose(file_fd);

    fileContent[file_size] = 0;

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

    char *otherDetails;
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
    }
    // else return false
}

char *AppDetails::toString()
{

    char *finalString = (char *)malloc(1000); //change it
    sprintf(finalString, "%d %s %s %s %s %s %s %s %s %s %s", this->id, this->owner, this->name, this->about, this->author, this->websiteLink, this->systemRequirements, this->price, this->ramMemory, this->version, this->otherDetails);
    return finalString;
}

void writeInFile(char *output_string)
{
    FILE *file_fd = fopen(apps_file, "a");
    strcat(output_string, "\n");
    fprintf(file_fd, "%s", output_string);
    fclose(file_fd);
}
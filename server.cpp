
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

void readFromSocket(char *buff, int fd);
void writeInSocket(char buffer[], int fd);
void handle_child(int client_fd, char *msg);
char *getInputCommand(char *);
int checkExistingUser(char *);

char *readFile(char *);

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
    FILE *file_fd = fopen(filename, "r");
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
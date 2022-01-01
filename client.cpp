/* Sa se realizeze un server pentru un depozit (repository) de aplicatii software. 
Adaugarea de noi aplicatii se va putea realiza de oricare client, specificindu-se cerintele hardware
 si/sau cele software (e.g., memorie RAM, procesor, platforma, biblioteci suplimentare etc.) pentru rularea acelei aplicatii. 
 Serverul va oferi facilitati de cautare a unei aplicatii, in functie de
  diverse criterii (nume, cerinte, producator, statut - open source, freeware, shareware etc.).*/

#include <sys/types.h>
#include <sys/stat.h>
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
#include <iostream>
#include <arpa/inet.h>
#define SIZE 1000

int port;
int socket_descriptor;
void writeInSocket(char buffer[], int fd);
void readFromSocket(char *buff, int fd);
void handle_signal(int sig);
char *readFile(char *file);

int main(int argc, char *argv[])
{

    struct sockaddr_in server;
    char msg[SIZE];
    int number;

    if (argc != 3)
    {
        printf("Parametrii insuficienti.Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(socket_descriptor, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }
    signal(SIGINT, handle_signal);
    while (1)
    {
        int check_download = 0;
        bzero(msg, SIZE);
        printf("[client]Introduceti comanda: ");
        fflush(stdout);
        read(0, msg, SIZE);
        writeInSocket(msg, socket_descriptor);
        if (strstr(msg, "downloadApp:"))
            check_download = 1;
        readFromSocket(msg, socket_descriptor);
        printf("[client]Mesajul primit este: %s\n", msg);
        if (strstr(msg, "quit"))
        {
            printf("[client]Mesajul primit este: %s.Byeeee\n", msg);
            close(socket_descriptor);
            exit(0);
        }
        if (check_download == 1)
        {
            char local_path[256] = "./";
            char *contentFile = readFile(msg);
            int size = strlen(contentFile); // save the size of the content of file

            char *nameOfFile;
            nameOfFile = strrchr(msg, '/') + 1;

            strcat(local_path, nameOfFile); //add to path

            int open_fd = open(local_path, O_CREAT | O_WRONLY | O_TRUNC, 0777); //create file
            write(open_fd, contentFile, size);
        }
    }
    close(socket_descriptor);
}

void writeInSocket(char buffer[], int fd)
{

    if (write(fd, buffer, SIZE) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
        //return errno;
    }
}
void readFromSocket(char *buff, int fd)
{
    bzero(buff, SIZE);
    if (read(fd, buff, SIZE) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        //return errno;
    }
}

void handle_signal(int sig)
{
    char msg[SIZE];
    bzero(msg, SIZE);
    strcpy(msg, "quit");
    writeInSocket(msg, socket_descriptor);
    readFromSocket(msg, socket_descriptor);
    exit(0);
}
char *readFile(char *file)
{
    FILE *file_fd;
    if ((file_fd = fopen(file, "r")) == NULL)
        printf("eroare deschidere fisier\n"); //open file in read mode

    fseek(file_fd, 0, SEEK_END);     //change the position at the end of the file
    long file_size = ftell(file_fd); //take the position
    fseek(file_fd, 0, SEEK_SET);     //go back at the beginning of the file

    char *fileContent = (char *)malloc(file_size + 1);
    fread(fileContent, 1, file_size, file_fd); //read the content
    fclose(file_fd);

    fileContent[file_size] = 0;

    return fileContent;
}
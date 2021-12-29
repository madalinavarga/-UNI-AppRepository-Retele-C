/* Sa se realizeze un server pentru un depozit (repository) de aplicatii software. 
Adaugarea de noi aplicatii se va putea realiza de oricare client, specificindu-se cerintele hardware
 si/sau cele software (e.g., memorie RAM, procesor, platforma, biblioteci suplimentare etc.) pentru rularea acelei aplicatii. 
 Serverul va oferi facilitati de cautare a unei aplicatii, in functie de
  diverse criterii (nume, cerinte, producator, statut - open source, freeware, shareware etc.).*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#define SIZE 1000

int port;
void writeInSocket(char buffer[], int fd);
void readFromSocket(char *buff, int fd);
void handle_signal(int sig);

int main(int argc, char *argv[])
{

    int socket_descriptor;
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
    signal(SIGQUIT, handle_signal);
    while (1)
    {
        bzero(msg, SIZE);
        printf("[client]Introduceti comanda: ");
        fflush(stdout);
        read(0, msg, SIZE);
        writeInSocket(msg, socket_descriptor);
        readFromSocket(msg, socket_descriptor);
        printf("[client]Mesajul primit este: %s\n", msg);
        if (strstr(msg, "quit"))
        {
            printf("[client]Mesajul primit este: %s.Byeeee\n", msg);
            close(socket_descriptor);
            exit(0);
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

    signal(sig, SIG_DFL);
}


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
#include <list>

#define FALSE 0
#define TRUE 1
#define PORT 4000
#define SIZE 1000

char config_file[] = "config.txt";
char apps_file[] = "apps.txt";
using namespace std;
int id_g = 0, check_param = 0;
char userName[50];
class AppDetails
{
public:
    int id;
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
    char *src_file;

    AppDetails(char *owner);
    AppDetails(){};

    void setFromtxtFile(char *fileName);
    void setField(char field[], char value[]);
    void setFromCsvLine(char *line);
    char *toString();
};

list<AppDetails> getListOfApps();
void readFromSocket(char *buff, int fd);
void writeInSocket(char buffer[], int fd);
void handler_client(int client_fd, char *msg);
char *getInputCommand(char *);
int checkExistingUser(char *);
char *readFile(char *file);
void writeInFile(char *output_string, const char *file);
char *getFirstParameter(char *givenString);
char *getSecondParameter(char *subString);
int checkExistingUserNameOnly(char *nameToFind);
bool validPassword(char *givenPass);
bool isValidField(char *field, char *value, AppDetails app);
int sizeOfFile(char *filename);
int validParameters(char *inputString, int number);

int main(int argc, char *argv[])
{

    struct sockaddr_in server;
    struct sockaddr_in clientStruct;
    int sd;
    char msg[SIZE];

    //reuse
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

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
        socklen_t length;
        length = sizeof(clientStruct);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&clientStruct, &length);
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
            //parent
            close(client);
        }
        else if (pid_general == 0)
        {
            //son
            close(sd);
            handler_client(client, msg);
        }
    }
}

void handler_client(int client_fd, char *msg)
{
    int isLogged = FALSE;
    while (1)
    {
        bzero(msg, SIZE);
        list<AppDetails> listOfApps = getListOfApps(); //create list of all apps uploaded
        id_g = listOfApps.end()->id;                   //set the last id nr
        readFromSocket(msg, client_fd);                //read msg from client
        printf("mesaj:%s\n", msg);
        if (strstr(msg, "quit")) // quit
        {

            writeInSocket(msg, client_fd);
            close(client_fd);
            exit(0);
        }
        else if (strstr(msg, "login:")) // login:nume parola
        {
            check_param = validParameters(msg, 1);
            if (check_param == 1)
            {
                if (isLogged == FALSE)
                {
                    strcpy(userName, " ");
                    char *userAccountDetails = getInputCommand(msg);   // return username password
                    int found = checkExistingUser(userAccountDetails); // search if username and password exists
                    if (found != 0)                                    //yes
                    {

                        isLogged = TRUE;
                        char *user_ptr = getFirstParameter(userAccountDetails); // save the username for next requests
                        strcpy(userName, user_ptr);
                        strcpy(msg, "Logged in\n");
                        writeInSocket(msg, client_fd); // return succes msg
                    }
                    else // user doesn't exist
                    {
                        strcpy(msg, "User does not exist\n");
                        writeInSocket(msg, client_fd);
                    }
                }
                else
                {
                    strcpy(msg, "already logged in\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "logout")) //logout
        {
            if (isLogged == TRUE)
            {
                isLogged = FALSE;
                strcpy(msg, "Logged out\n");
                writeInSocket(msg, client_fd);
            }
            else
            {
                strcpy(msg, "you are not logged in\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "addNewApp:")) //addNewApp:file
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {

                if (isLogged == TRUE) // must be logged in
                {
                    listOfApps = getListOfApps();          //refresh
                    id_g = listOfApps.end()->id;           //set the last id nr
                    char *fileName = getInputCommand(msg); // take the parameter: a file with the new app details
                    AppDetails app(userName);              //create an object
                    //set the object attributes
                    app.setFromtxtFile(fileName);
                    int dir_error = 0, open_fd = 0;
                    if (strcmp(app.src_file, "default") != 0) // check is open source
                    {
                        //save the file
                        char path_server[256];
                        char id_string[10];
                        sprintf(id_string, "%d", app.id); //convert int id to string

                        char *contentFile = readFile(app.src_file); // take the content of file
                        int size = strlen(contentFile);             // save the size of the content of file
                        snprintf(path_server, 256, "/home/madalinavarga21/Desktop/Retele/ProiectFinal/fisiere_salvate/%s/", id_string);
                        dir_error = mkdir(path_server, 0777); //create a new directory with the id name

                        char *nameOfFile;
                        nameOfFile = strrchr(app.src_file, '/') + 1;
                        printf("nume returnat: %s", nameOfFile);

                        strcat(path_server, nameOfFile); //add to path
                        app.src_file = (char *)malloc(strlen(path_server));
                        strcpy(app.src_file, path_server);
                        open_fd = open(path_server, O_CREAT | O_WRONLY | O_TRUNC, 0777); //create file
                        int check = write(open_fd, contentFile, size);                   // write the content in the new file
                        if (check < 0)
                            printf("eroare scriere in fisier\n");
                    }
                    if (dir_error == -1 || open_fd == -1)
                        strcpy(msg, "Error to copy the files");
                    else
                        snprintf(msg, SIZE, "App loaded with id: %d", app.id);
                    char *output_string = app.toString();
                    writeInFile(output_string, apps_file); //save the new app
                    writeInSocket(msg, client_fd);
                }
                else
                {
                    strcpy(msg, "You must be logged in to add a new app\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "newUser:")) //newUser:nume parola
        {
            check_param = validParameters(msg, 1);
            if (check_param == 1)
            {
                char *new_user_details = getInputCommand(msg); // return username password
                char *wanted_name = getFirstParameter(new_user_details);
                int found = checkExistingUserNameOnly(wanted_name);

                if (found == 0)
                {
                    // the wanted username is avaible => continue

                    char *password = getSecondParameter(new_user_details);
                    int check_password = validPassword(password);

                    if (check_password == 1)
                    {
                        //good pass => create account
                        writeInFile(new_user_details, config_file);
                        strcpy(msg, "user created\n");
                        writeInSocket(msg, client_fd);
                    }
                    else
                    {
                        strcpy(msg, "incorrect password format\n");
                        writeInSocket(msg, client_fd);
                    }
                }
                else
                {
                    strcpy(msg, "username already exists\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "seeApp:")) //seeApp:id
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {
                char *id_app = getInputCommand(msg); //take the id and search it in the list
                int id = atoi(id_app);
                int found = 0;
                for (auto app = listOfApps.begin(); app != listOfApps.end(); app++) //check if the id is in the list
                {
                    if (app->id == id)
                    {
                        strcpy(msg, app->toString()); //convert it to string and return the details to the client
                        found = 1;
                        break;
                    }
                }
                if (found == 0)
                    strcpy(msg, "ivalid id\n");

                writeInSocket(msg, client_fd);
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "update:")) // update:id file with the new details
        {
            check_param = validParameters(msg, 1);
            if (check_param == 1)
            {
                if (isLogged == TRUE) //must be logged in
                {
                    char *parameters = getInputCommand(msg);
                    char *id_app = getFirstParameter(parameters);      //take the id
                    char *fileUpdate = getSecondParameter(parameters); //take the file name
                    int id = atoi(id_app);
                    int found = 0, unathorised = 0;
                    FILE *file_fd = fopen(apps_file, "w+");
                    fseek(file_fd, 0, SEEK_SET);
                    for (auto app = listOfApps.begin(); app != listOfApps.end(); app++) //fie sterg tot si scriu iar.. fie parcurg lista de 2 ori?!
                    {
                        if (app->id == id) // check if the id is in the list
                        {
                            if (strcmp(app->owner, userName) == 0) // check if the user is the owner
                            {
                                found++;

                                if (strcmp(app->src_file, "default") != 0)
                                {

                                    char old_file[256]; //take the old file path
                                    strcpy(old_file, app->src_file);

                                    app->setFromtxtFile(fileUpdate); //reset the attributes

                                    char path_server[256];
                                    char *contentFile = readFile(app->src_file); // take the content of file
                                    int size = strlen(contentFile);              // save the size of the content of file
                                    snprintf(path_server, 256, "/home/madalinavarga21/Desktop/Retele/ProiectFinal/fisiere_salvate/%s/", id_app);
                                    char *nameOfFile;
                                    nameOfFile = strrchr(app->src_file, '/') + 1;

                                    strcat(path_server, nameOfFile); //add to path
                                    app->src_file = (char *)malloc(strlen(path_server));
                                    strcpy(app->src_file, path_server);
                                    int open_fd = open(path_server, O_CREAT | O_WRONLY | O_TRUNC, 0777); //create file

                                    int check = write(open_fd, contentFile, size);
                                    if (check < 0)
                                        printf("eroare scriere in fisier\n");
                                    printf("nume la ce sterg:\n %s", old_file);
                                    remove(old_file);
                                }
                                else
                                    app->setFromtxtFile(fileUpdate); //reset the attributes
                            }
                            else
                                unathorised = 1;
                        }

                        char *output_string = app->toString();
                        fprintf(file_fd, "%s\n", output_string);
                    }
                    strcpy(msg, "app updated\n");
                    fclose(file_fd);

                    if (found == 0)
                        strcpy(msg, "ivalid id\n");

                    if (unathorised == 1)
                        strcpy(msg, "Only the owner has the authorization to make any change\n");

                    writeInSocket(msg, client_fd);
                }

                else
                {
                    strcpy(msg, "you must to be logged in\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "searchApps:")) //searchApps:filename
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {
                char returnedString[SIZE] = "";
                char *nameOfFile = getInputCommand(msg);
                char delim[] = "{},:\" \t\n";
                bool valid = true;
                bool foundOneApp = false;

                for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                {
                    char *filtersDetails = readFile(nameOfFile); // filters
                    char *field = strtok(filtersDetails, delim);
                    char *value = strtok(NULL, delim);

                    while (field) // for each app check all filters
                    {
                        valid = isValidField(field, value, *app); // return if the current filed and value is the wanted one

                        if (valid == false)
                        {
                            break;
                        }
                        field = strtok(NULL, delim);
                        value = strtok(NULL, delim);
                    }
                    if (valid == true) //match
                    {
                        foundOneApp = true;
                        char *output_string = app->toString();
                        strcat(returnedString, output_string); // add it to the returned string
                        strcat(returnedString, "\n\n");
                    }
                }

                if (foundOneApp == false)
                {
                    strcpy(returnedString, "Didn't find any app\n");
                }

                writeInSocket(returnedString, client_fd);
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "deleteApp:")) //id
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {
                int found = 0, unathorised = 0;

                if (isLogged == TRUE) //must be logged in
                {
                    char *parameter = getInputCommand(msg);
                    int id = atoi(parameter);
                    int line_id = 0;
                    FILE *file_fd_w, *file_fd_r;
                    char path_file[256];

                    for (auto app = listOfApps.begin(); app != listOfApps.end(); app++) //search for the id
                    {
                        if (app->id == id)
                        {
                            if (strcmp(app->owner, userName) == 0) //check the username = owner
                            {
                                strcpy(path_file, app->src_file);
                                //open the apps file in read mode and a temporary app in write
                                if ((file_fd_r = fopen(apps_file, "r")) == NULL || (file_fd_w = fopen("delete.tmp", "w+")) == NULL)
                                    printf("eroare deschidere fisier\n");

                                long file_size = sizeOfFile(apps_file);
                                char *fileContent = (char *)malloc(file_size + 1);
                                while (1) //while i still have lines to read
                                {

                                    char *linie = fgets(fileContent, file_size, file_fd_r);
                                    if (linie == NULL)
                                        break;
                                    char aux[100];
                                    strcpy(aux, linie); //copy the line to find the id of each app

                                    line_id = atoi(strtok(aux, ";"));

                                    if (line_id != id) //if the given id is not the current one, write the line in file
                                        fputs(fileContent, file_fd_w);
                                }
                                fclose(file_fd_w);
                                fclose(file_fd_r);
                                remove(apps_file);
                                rename("delete.tmp", apps_file);

                                free(fileContent);
                                if (strcmp(path_file, "default") != 0)
                                {
                                    remove(path_file);
                                }

                                found = 1;
                                strcpy(msg, "app deleted\n");
                            }
                            else
                            {
                                strcpy(msg, "Only the owner has the authorization to make any change\n");
                                unathorised = 1;
                            }
                        }
                    }

                    if (found == 0)
                        strcpy(msg, "invalid id\n");
                    if (unathorised == 1)
                        strcpy(msg, "Only the owner has the authorization to make any change\n");
                    writeInSocket(msg, client_fd);
                }
                else
                {
                    strcpy(msg, "you must to be logged in\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "seeAllApps:")) //page nr
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {
                char *firstParam = getInputCommand(msg);
                int page_number = atoi(firstParam);  // take the page number
                int first_app = page_number * 5 - 5; //5 apps per page
                int line_index = 0, count = 5;
                int size = sizeOfFile(apps_file);
                char returnedString[size] = ""; //a string with all apps

                for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                {
                    if (line_index == first_app) //find the first app wanted
                    {
                        if (count > 0) // take the 5 apps wanted in returned string
                        {
                            char *output_string = app->toString();
                            strcat(returnedString, output_string);
                            strcat(returnedString, "\n");
                        }

                        first_app++;
                        count--;
                    }
                    line_index++;
                }
                if (strlen(returnedString) < 1)
                {
                    strcpy(msg, "No apps avaible\n");
                    writeInSocket(msg, client_fd);
                }
                else
                    writeInSocket(returnedString, client_fd);
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "downloadApp:")) //downloadApp:id
        {
            check_param = validParameters(msg, 0);
            if (check_param == 1)
            {
                if (isLogged == TRUE)
                {
                    char *id_app = getInputCommand(msg); //take the id and search it in the list
                    int id = atoi(id_app);
                    int found = 0;
                    for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                    {
                        if (app->id == id)
                        {
                            found = 1;
                            if (strcmp(app->src_file, "default") == 0) // check if the app is open source
                                strcpy(msg, "this app is not open source\n");
                            else
                            {
                                strcpy(msg, app->src_file);
                            }
                        }
                    }
                    if (found == 0)
                        strcpy(msg, "app doesn't exist\n");

                    writeInSocket(msg, client_fd);
                }
                else
                {
                    strcpy(msg, "You are not logged in\n");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "incorrect number of parameters\n");
                writeInSocket(msg, client_fd);
            }
        }
        else
        {
            strcpy(msg, "command doesn't exist!\n");
            writeInSocket(msg, client_fd);
        }
    }
}
char *getInputCommand(char *inputString) //command:parameters*
{

    char *subString;
    subString = strrchr(inputString, ':') + 1;
    // subString[strlen(subString) - 1] = '\0';
    return subString;
}

int checkExistingUser(char *usernamesAndPassword)
{
    if (strstr(usernamesAndPassword, " ") == NULL) // only one parameter
    {
        return 0;
    }

    strcat(usernamesAndPassword, "\n");

    char *listOfUsernamesAndPasswords = readFile(config_file); //madalina parola
    if (strstr(listOfUsernamesAndPasswords, usernamesAndPassword) != NULL)
    {
        return 1;
    }
    return 0;
}
void readFromSocket(char *buff, int fd)
{
    bzero(buff, SIZE);
    if (read(fd, buff, SIZE) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
    }
}

void writeInSocket(char buffer[], int fd)
{

    if (write(fd, buffer, SIZE) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
    }
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
int sizeOfFile(char *filename)
{
    FILE *file_fd;
    if ((file_fd = fopen(filename, "r")) == NULL)
        printf("eroare deschidere fisier\n"); //open file in read mode

    fseek(file_fd, 0, SEEK_END);     //change the position at the end of the file
    long file_size = ftell(file_fd); //take the position
    fseek(file_fd, 0, SEEK_SET);
    return file_size;
}

void writeInFile(char *output_string, const char *file)
{
    FILE *file_fd = fopen(file, "a"); // append mode
    fprintf(file_fd, "%s\n", output_string);
    fclose(file_fd);
}

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

int checkExistingUserNameOnly(char *nameToFind)
{
    FILE *configFd = fopen(config_file, "r"); // open file

    char wordAux[100];
    char nume[50];
    int wordAuxCount = 0; // password on odd position
    int count = 0;

    while (1)
    {
        if (fscanf(configFd, "%s", wordAux) == EOF) // read word by word
            break;
        else
        {
            wordAuxCount++;
            if (strcmp(wordAux, nameToFind) == 0 && wordAuxCount % 2 == 1) // compare username with current word
            {
                count++;
                break;
            }
        }
    }
    fclose(configFd);
    return count;
}

bool validPassword(char *password) // must be >8 characters + 1+ numbers 1+uppercase 1+lowercase
{

    int count_letter = 0, count_LETTER = 0, count_digits = 0, count_length = 0, something_else = 0;
    count_length = strlen(password);

    if (count_length < 8) //check length
        return false;

    for (int i = 0; i < count_length; i++) //check the content
    {

        if (isupper(password[i]))
            count_LETTER++;
        else if (islower(password[i]))
            count_letter++;
        else if (isdigit(password[i]))
            count_digits++;
        else
            something_else++;
    }
    if (count_LETTER > 0 && count_letter > 0 && something_else == 0 && count_digits > 0)
        return true;
    else
        return false;
}

list<AppDetails> getListOfApps()
{
    char *contents = readFile(apps_file); // read the file with apps
    list<AppDetails> listOfApps;          //create a list
    char delim_apps[] = "\n\r";
    char *appDetails = strtok_r(contents, delim_apps, &contents);
    while (appDetails != nullptr) // for each app create an object and add it to the list
    {
        AppDetails app;

        app.setFromCsvLine(appDetails);
        listOfApps.push_back(app);
        appDetails = strtok_r(NULL, delim_apps, &contents);
    }

    return listOfApps;
}

bool isValidField(char *field, char *value, AppDetails app)
{

    if (strcmp(field, "name") == 0)
    {
        if (strstr(app.name, value))
            return true;
        else
            return false;
    }
    if (strcmp(field, "systemRequirements") == 0)
    {
        if (strstr(app.systemRequirements, value))
            return true;
        else
            return false;
    }
    if (strcmp(field, "price") == 0)
    {
        if (strstr(app.price, value))
            return true;
        else
            return false;
    }
    if (strcmp(field, "ramMemory") == 0)
    {

        if (strstr(app.ramMemory, value))

            return true;
        else
            return false;
    }
    if (strcmp(field, "version") == 0)
    {
        if (strstr(app.version, value))
            return true;
        else
            return false;
    }
    if (strcmp(field, "author") == 0)
    {
        if (strstr(app.author, value))
            return true;
        else
            return false;
    }

    return false;
}
int validParameters(char *inputString, int number)
{
    int size = strlen(inputString);
    inputString[size - 1] = '\0';

    int count_space = 0;
    int count_param = 0, j = 0;
    for (int i = 0; i < size; i++)
    {

        if (inputString[i] == ' ')
            count_space++;
        if (number == 0)
            if (inputString[i] == ':')
                j = i;
    }
    if (number == 0)
    {
        for (int i = j + 1; i < size; i++)
        {
            if (inputString[i] != '\0')
                count_param++;
        }
    }

    if (number == 0)
    {
        if (count_param > 0)
            return 1;
    }
    else

        if (count_space == number)
        return 1;

    return 0;
}
char *AppDetails::toString()
{

    char *finalString = (char *)malloc(SIZE); //change it
    sprintf(finalString, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;", this->id, this->owner, this->name, this->about, this->author, this->websiteLink, this->systemRequirements, this->price, this->ramMemory, this->version, this->otherDetails, this->src_file);
    return finalString;
}

AppDetails::AppDetails(char *owner)
{
    this->owner = (char *)malloc(strlen(owner) + 1);
    strcpy(this->owner, owner);
    this->id = id_g;
    id_g++;
}

void AppDetails::setFromtxtFile(char *fileName)
{
    char *contents = readFile(fileName);
    char delim[] = ":\n";

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
    }
    else if (strcmp(field, "otherDetails") == 0)
    {

        this->otherDetails = (char *)malloc(strlen(value) + 1);
        strcpy(this->otherDetails, value);
    }
    else if (strcmp(field, "src_file") == 0)
    {

        this->src_file = (char *)malloc(strlen(value) + 1);
        strcpy(this->src_file, value);
    }
}

void AppDetails::setFromCsvLine(char *line)
{

    char delim_field[] = ";";
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

    field = strtok(NULL, delim_field);
    this->src_file = (char *)malloc(strlen(field) + 1);
    strcpy(this->src_file, field);
}

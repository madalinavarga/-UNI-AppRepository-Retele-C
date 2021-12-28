
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

char config_file[] = "config.txt";
char apps_file[] = "apps.txt";
using namespace std;
int id = 0;
char userName[50];
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

int main(int argc, char *argv[])
{

    struct sockaddr_in server;
    struct sockaddr_in clientStruct;
    int sd;
    char msg[1000];

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
            while (waitpid(-1, NULL, WNOHANG))
                ;
            continue;
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
        bzero(msg, 100);
        list<AppDetails> listOfApps = getListOfApps(); //create list of apps uploaded
        readFromSocket(msg, client_fd);                //read msg from client
        printf("\nmesaj:%s", msg);
        if (strstr(msg, "quit")) // if quit => exit
        {
            printf("[client]Mesajul primit este: %s. byee client\n", msg);
            writeInSocket(msg, client_fd);
            close(client_fd);
            exit(0);
        }
        else if (strstr(msg, "login:"))
        {
            if (isLogged == FALSE)
            {
                strcpy(userName, " ");
                char *userAccountDetails = getInputCommand(msg); // return username password

                int found = checkExistingUser(userAccountDetails); // search if username and password exists
                printf("%d\n", found);
                if (found != 0) //yes
                {

                    isLogged = TRUE;
                    char *user_ptr = getFirstParameter(userAccountDetails); // save the username for next requests
                    strcpy(userName, user_ptr);
                    strcpy(msg, "Logged in");
                    writeInSocket(msg, client_fd); // return succes msg
                }
                else //no
                {
                    strcpy(msg, "User does not exist");
                    writeInSocket(msg, client_fd);
                }
            }
            else
            {
                strcpy(msg, "already logged in");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "logout"))
        {
            if (isLogged == TRUE)
            {
                isLogged = FALSE;
                strcpy(msg, "Logged out");
                writeInSocket(msg, client_fd);
            }
            else
            {
                strcpy(msg, "you are not logged in");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "addNewApp:")) //by file
        {
            if (isLogged == TRUE) // must be logged in
            {
                char *fileName = getInputCommand(msg); // take a file with the new app details
                AppDetails app(userName);              //create an object
                printf("fileName: %s\n", fileName);
                app.setFromJsonFile(fileName);
                char *output_string = app.toString();
                printf("output: %s\n", output_string);
                writeInFile(output_string, apps_file);
                strcpy(msg, "App loaded");
                writeInSocket(msg, client_fd);
            }
            else
            {

                strcpy(msg, "You must be logged in to add a new app\n");
                writeInSocket(msg, client_fd);
            }
        }
        else if (strstr(msg, "newUser:"))
        {

            char *new_user_details = getInputCommand(msg); // return username password
            printf("<%s> - user datails\n", new_user_details);

            char *wanted_name = getFirstParameter(new_user_details); //de modificat somthing wrong
            //printf("<%s> name\n", wanted_name);
            printf("\nDupa ce pun in username\n\n");

            int found = checkExistingUserNameOnly(wanted_name);
            printf("found: %d\n", found);

            if (found == 0)
            {
                // numele nu exista, poti face contul daca parola e ok

                char *password = getSecondParameter(new_user_details);
                printf("%s pass is \n", password);
                int check_password = validPassword(password);
                if (check_password == 1)
                {
                    //utilizator + parola ok
                    writeInFile(new_user_details, config_file);
                    strcpy(msg, "user created\n");
                    writeInSocket(msg, client_fd);
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
        else if (strstr(msg, "seeApp:")) //by id
        {
            char *id_app = getInputCommand(msg); //take the id and search it in the list
            int id = atoi(id_app);
            int found = 0;
            for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
            {
                if (app->id == id)
                {
                    strcpy(msg, app->toString()); //convert it to string and return the details to the user
                    found = 1;
                    break;
                }
            }
            if (found == 0)
                strcpy(msg, "ivalid id");

            writeInSocket(msg, client_fd);
        }
        else if (strstr(msg, "update:")) // parameters: id + file with the new details
        {
            if (isLogged == TRUE)
            {
                char *parameters = getInputCommand(msg);
                char *id_app = getFirstParameter(parameters);
                printf("parametru 1 <%s>\n", id_app);
                char *fileUpdate = getSecondParameter(parameters);
                printf("parametru 2 <%s>\n", fileUpdate);
                int id = atoi(id_app);
                int found = 0;
                for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                {
                    if (app->id == id)
                    {
                        if (strcmp(app->owner, userName) == 0)
                        {

                            FILE *file_fd = fopen(apps_file, "w+");
                            fseek(file_fd, 0, SEEK_SET);

                            for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                            {
                                if (app->id == id)
                                {

                                    app->setFromJsonFile(fileUpdate);

                                    for (auto i = listOfApps.begin(); i != listOfApps.end(); i++)
                                    {
                                        char *output_string = i->toString();
                                        fprintf(file_fd, "%s\n", output_string);
                                    }
                                }
                            }
                            fclose(file_fd);
                            found++;
                            strcpy(msg, "app updated");
                        }
                        else
                            strcpy(msg, "Only the owner has the authorization to make any change");
                    }
                }

                if (found == 0)
                    strcpy(msg, "ivalid id");

                writeInSocket(msg, client_fd);
            }
            else
            {
                strcpy(msg, "you must to be logged in");
                writeInSocket(msg, client_fd);
            }
        }

        else if (strstr(msg, "searchApps:")) //filename
        {
            char returnedString[100] = "";
            printf("Intru in search apps\n");
            printf("Mesajul: %s\n", msg);
            char *nameOfFile = getInputCommand(msg);
            char delim[] = "{},:\" \t\n";
            bool valid = true;

            for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
            {
                char *filtersDetails = readFile(nameOfFile); // filters
                char *field = strtok(filtersDetails, delim);
                char *value = strtok(NULL, delim);

                while (field)
                {
                    valid = isValidField(field, value, *app); // return if the current filed and value is the wanted one

                    if (valid == false)
                    {
                        break;
                    }
                    field = strtok(NULL, delim);
                    value = strtok(NULL, delim);
                }
                if (valid == true)
                {
                    char *output_string = app->toString();

                    strcat(returnedString, output_string);
                    strcat(returnedString, "\n\n");
                }
            }

            writeInSocket(returnedString, client_fd);
        }
        else if (strstr(msg, "deleteApp:"))
        {
            if (isLogged == TRUE)
            {
                char *parameter = getInputCommand(msg);
                int id = atoi(parameter);
                for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
                {
                    if (app->id == id)
                    {
                        if (strcmp(app->owner, userName) == 0)
                        {
                        }
                    }
                }
                // else
                // {
                //     strcpy(msg, "Only the owner has the authorization to make any change");
                // }
            }
        }
        else
        {
            strcpy(msg, "command doesn't exist!");
        }
        writeInSocket(msg, client_fd);
    }
}
char *getInputCommand(char *inputString) //command:parameters*
{

    char *subString;
    subString = strrchr(inputString, ':') + 1;
    subString[strlen(subString) - 1] = '\0';
    return subString;
}

int checkExistingUser(char *wordToFind) //
{
    char *usernamesAndPasswords = readFile(config_file);
    if (strstr(usernamesAndPasswords, wordToFind) != NULL)
    {
        return 1;
    }
    return 0;
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

bool validPassword(char *givenPass) // must be >8 characters + 1+ numbers 1+uppercase 1+lowercase
{

    int count_letter = 0, count_LETTER = 0, count_digits = 0, count_length = 0, something_else = 0;
    count_length = strlen(givenPass);
    char password[count_length];
    strcpy(password, givenPass);

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
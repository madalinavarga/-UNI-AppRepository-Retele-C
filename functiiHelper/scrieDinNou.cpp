
#include "AppClass.h"

#define FALSE 0
#define TRUE 1
#define PORT 4449
char config_file[] = "config.txt";
char apps_file[] = "apps.txt";
using namespace std;
int id = 0;
char *readFile(char *file);
bool isValidField(char *field, char *value, AppDetails app);

list<AppDetails> getListOfApps();

int main()
{
    list<AppDetails> listOfApps = getListOfApps(); // am lista de aplicatii
    // char *parameter = getInputCommand(msg);
    char parameter[] = "1";
    int id = atoi(parameter);

    FILE *file_fd = fopen(apps_file, "w+");
    fseek(file_fd, 0, SEEK_SET);
    for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
    {
        if (app->id == id)
        {

            app = ++app;
        }
        fprintf(file_fd, "%s\n", app->toString());
    }

    // int iid = 1;
    // FILE *file_fd = fopen(apps_file, "w+");
    // fseek(file_fd, 0, SEEK_SET);
    // char fisierUpdate[] = "test.json";
    // for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
    // {
    //     if (app->id == iid)
    //     {

    //         app->setFromJsonFile(fisierUpdate);

    //         for (auto i = listOfApps.begin(); i != listOfApps.end(); i++)
    //         {
    //             char *output_string = i->toString();
    //             fprintf(file_fd, "%s\n", output_string);
    //         }
    //     }
    // }
    // fclose(file_fd);

    // search apps => parameter a file

    // char numeFisier[] = "update.txt"; //read input
    // char delim[] = "{},:\" \t\n";
    // bool ok = true;

    // char buffer[1000] = "";

    // //pentru fiecare field il cauti si vezi daca este ok => search in lista of app
    // for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
    // {
    //     char *filtersDetails = readFile(numeFisier); //=> string with all
    //     char *field = strtok(filtersDetails, delim);
    //     char *value = strtok(NULL, delim);

    //     while (field)
    //     {
    //         ok = isValidField(field, value, *app);
    //         printf("%d\n", ok);
    //         if (ok == false)
    //         {

    //             break;
    //         }
    //         field = strtok(NULL, delim);
    //         value = strtok(NULL, delim);
    //     }
    //     if (ok == true)
    //     {
    //         char *output_string = app->toString();

    //         strcat(buffer, output_string);
    //         strcat(buffer, "\n");
    //     }
    // }

    // printf("%s\n", buffer);
}

list<AppDetails> getListOfApps()
{
    char *contents = readFile(apps_file); // all content
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

    return listOfApps;
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

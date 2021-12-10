
#include "AppClass.h"

#define FALSE 0
#define TRUE 1
#define PORT 4449
char config_file[] = "config.txt";
char apps_file[] = "apps.txt";
using namespace std;
int id = 0;
char *readFile(char *file);

list<AppDetails> getListOfApps();

int main()
{
    list<AppDetails> listOfApps = getListOfApps(); // am lista de aplicatii
    int iid = 1;
    FILE *file_fd = fopen(apps_file, "w+");
    fseek(file_fd, 0, SEEK_SET);
    char fisierUpdate[] = "update.txt";
    for (auto app = listOfApps.begin(); app != listOfApps.end(); app++)
    {
        if (app->id == iid)
        {
            char *line = readFile(fisierUpdate);
            app->setFromCsvLine(line);

            for (auto i = listOfApps.begin(); i != listOfApps.end(); i++)
            {
                char *output_string = i->toString();
                fprintf(file_fd, "%s\n", output_string);
            }
        }
    }
    fclose(file_fd);
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

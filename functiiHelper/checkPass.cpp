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
#include <ctype.h>
using namespace std;
char *getUserPassword(char *subString)
{
    char delim[] = " ";
    char *ptr_name = strtok(subString, delim);
    ptr_name = strtok(NULL, delim);
    return ptr_name;
}

bool validPassword(char *givenPass)
{

    int count_letter = 0, count_LETTER = 0, count_digits = 0, count_length = 0, something_else = 0;
    count_length = strlen(givenPass);
    char password[count_length];
    strcpy(password, givenPass);

    if (count_length < 8)
        return false;

    for (int i = 0; i < count_length; i++)
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

int main()
{
    char nume[] = "Madalina Era2aaaa1";
    char *parola = getUserPassword(nume);

    int ok = validPassword(parola);
    printf("%d + <%s>\n", ok, parola);
}
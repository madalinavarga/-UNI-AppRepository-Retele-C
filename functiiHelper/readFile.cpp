#include <iostream>
#include <istream>

using namespace std;

ifstream file("config.txt");
if (file.is_open())
{
    char line[80];
    while (getline(file, line, 80))
    {

        cout << line << endl;
    }
}
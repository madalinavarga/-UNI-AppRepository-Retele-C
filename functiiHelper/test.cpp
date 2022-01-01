#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

char *readFile(int fd)
{
    char *buf = (char *)malloc(sizeof(char) * 1000);
    int n = 0;
    while (1)
    {
        char ch;
        if (read(fd, &ch, 1) <= 0)
        {
            break;
        }
        buf[n++] = ch;
    }
    return buf;
}

void writeFile(int fd, char *buf)
{
    int n = write(fd, buf, 1000);
    if (n < 0)
    {
        perror("write");
        exit(1);
    }
}

int main()
{
    int fd = open("test.zip", O_RDONLY);
    char *buf = readFile(fd);

    printf("%s\n", buf);

    int fd2 = open("test2.zip", O_WRONLY | O_CREAT, 0777);
    writeFile(fd2, buf);
}
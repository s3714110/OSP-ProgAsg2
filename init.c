#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int init(char *filename)
{
    FILE *file;

    if ( (file = fopen(filename, "r")) )
    {
        fclose(file);
        printf("file exists");
    }
    else
    {
        printf("file doesn't exist");
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define DefaultFilesystemName "FS.notes"

int init(void)
{
    FILE *file;

    if ( (file = fopen(DefaultFilesystemName, "r")) )
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
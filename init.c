#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ValidFileCheck "NOTES V1.0"
#define MAX_LINE_LENGTH 255

int init(char *filename)
{
    FILE *file;

    if ((file = fopen(filename, "r")))
    {
        char test_line[MAX_LINE_LENGTH] = ValidFileCheck;
        
        char first_line[MAX_LINE_LENGTH] = {0};
        fgets(first_line, MAX_LINE_LENGTH, file);

        first_line[strcspn(first_line, "\n")] = '\0';

        if (strcmp(first_line, test_line) == 0)
        {
            printf("This is a valid notes file\n");
        }
        else
        {
            printf("This is not a valid notes file\n");
        }

        fclose(file);
        printf("file exists\n");
    }
    else
    {
        printf("file doesn't exist\n");
    }

    return 0;
}

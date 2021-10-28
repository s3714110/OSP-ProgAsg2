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
        printf("The notes file has been detected\n");
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
            fprintf(stderr, "This is not a valid notes file. Please remove the notes file and run the program again\n");
            exit(EXIT_FAILURE);
        }

        fclose(file);
    }
    else
    {
        printf("Notes file not detected. Creating a new notes files...\n");
        file = fopen(filename, "w");
        fprintf(file, "%s\n", ValidFileCheck);
        fclose(file);
        printf("New file created\n");
    }

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 255

int count_slash(char *dir_name)
{

}

int index_of_last_slash(char *dir_name)
{

}

int duplicate_directory(char *dir_name, char *filename)
{
    int is_duplicate = 0;
    int line_count = 0;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((temp_file = fopen(filename, "r")))
    {
        while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
        {
            line_count++;
            temp_line[strcspn(temp_line, "\n")] = '\0';

            if (strcmp(dir_name, temp_line) == 0)
            {
                is_duplicate = 1;
            }
        }

        fclose(temp_file);
    }

    return is_duplicate;
}

int mkdir(char *filename, char *id)
{

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define BufferFileName "temp_buffer.notes"
#define MAX_LINE_LENGTH 255

int check_exist(char *name, char *filename)
{
    int exists = 0;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((temp_file = fopen(filename, "r")))
    {
        while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0';

            if (strcmp(name, temp_line) == 0)
            {   
                exists = 1;
            }
        }

        fclose(temp_file);
    }

    return exists;
}

int remove_file(char *filename, char *if_name)
{

 return EXIT_SUCCESS;   
}

int remove_dir(char *filename, char *id_name)
{

 return EXIT_SUCCESS;   
}



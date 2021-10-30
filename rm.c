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
    if (strlen(if_name) <= MAX_LINE_LENGTH - 2)
    {
        char internal_file_name[MAX_LINE_LENGTH] = {0};
        internal_file_name[0] = "@";
        strncpy(internal_file_name + 1, if_name, strlen(if_name));

        if(check_exist(internal_file_name, filename) == 1)
        {

        }
        else
        {
            fprintf(stderr, "Error! The given file name can not be found in the filesystem. Please try again!\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given file name exceeds the maximum length. Please try again!\n");
        exit(EXIT_FAILURE);
    }

 return EXIT_SUCCESS;   
}

int remove_dir(char *filename, char *id_name)
{
    if (strlen(id_name) <= MAX_LINE_LENGTH - 3)
    {
        char internal_dir_name[MAX_LINE_LENGTH] = {0};
        internal_dir_name[0] = '=';

        if (id_name[0] == '/')
        {
            strncpy(internal_dir_name + 1, id_name + 1, strlen(id_name));
        }
        else
        {
            strncpy(internal_dir_name + 1, id_name, strlen(id_name));
        }

        if (internal_dir_name[strcspn(internal_dir_name, "\0") - 1] != '/')
        {
            internal_dir_name[strcspn(internal_dir_name, "\0")] = '/';
        }

        if(check_exist(internal_dir_name, filename) == 1)
        {

        }
        else
        {
            fprintf(stderr, "Error! The given directory name can not be found in the filesystem. Please try again!\n");
            exit(EXIT_FAILURE);
        }

        
    }
    else
    {
        fprintf(stderr, "Error! The given directory name exceeds the maximum length. Please try again!\n");
        exit(EXIT_FAILURE);
    }


 return EXIT_SUCCESS;   
}



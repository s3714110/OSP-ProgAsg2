#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "mkdir.h"
#include "rm.h"

#define MAX_LINE_LENGTH 255

int index_of_last_slash(char *name)
{
    int last_slash = -1;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, name, MAX_LINE_LENGTH);

    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            last_slash = i;
        }
    }

    return last_slash;
}

int copy_in(char *filename, char *ef_name, char *if_name)
{
    if (strlen(if_name) <= MAX_LINE_LENGTH - 2)
    {
        char internal_file_name[MAX_LINE_LENGTH] = {0};
        internal_file_name[0] = '@';

        if (if_name[0] == '/')
        {
            strncpy(internal_file_name + 1, if_name + 1, strlen(if_name));
        }
        else
        {
            strncpy(internal_file_name + 1, if_name, strlen(if_name));
        }

        if (check_exist(internal_file_name, filename) == 1)
        {
            printf("File with name %s found on the filesystem %s. The program will now delete the old file ... \n", if_name, filename);
            remove_file(filename, if_name);
        }

        if (index_of_last_slash(if_name) > 0)
        {
            char id_name[MAX_LINE_LENGTH] = {0};
            strncpy(id_name, if_name, index_of_last_slash(if_name) + 1);

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

            if (duplicate_directory(internal_dir_name, filename) == 0)
            {
                printf("Directory %s not found on the filesystem %s. The program will now automatically create directories as needed ... \n", id_name, filename);
                make_dir(filename, id_name);
            }
        }

        FILE *file;
        FILE *external_file;

        if ((file = fopen(filename, "a")))
        {
            if ((external_file = fopen(ef_name, "r")))
            {

                fputs(internal_file_name, file);
                fputc('\n', file);

                char next_external_line[MAX_LINE_LENGTH - 2] = {0};
                while (fgets(next_external_line, MAX_LINE_LENGTH - 2, external_file))
                {
                    next_external_line[strcspn(next_external_line, "\n")] = '\0';
                    char next_internal_line[MAX_LINE_LENGTH] = {0};
                    next_internal_line[0] = ' ';

                    strncpy(next_internal_line + 1, next_external_line, strlen(next_external_line));

                    fputs(next_internal_line, file);
                    fputc('\n', file);
                }

                fclose(external_file);
            }
            else
            {
                fprintf(stderr, "Error! Can not access external file. Please try again\n");
                exit(EXIT_FAILURE);
            }

            fclose(file);
            printf("Content of external file %s has been copied into internal file %s on the filesystem %s\n", ef_name, if_name, filename);
        }
        else
        {
            fprintf(stderr, "Error! Can not access notes file. Please try again\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given internal file name exceeds the maximum length. Please try again!\n");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int copy_out(char *filename, char *if_name, char *ef_name)
{

    return EXIT_SUCCESS;
}

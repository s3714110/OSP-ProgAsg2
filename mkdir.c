#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 255

int count_slash(char *dir_name)
{
    int slashes = 0;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, dir_name, MAX_LINE_LENGTH);

    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            slashes++;
        }
    }

    return slashes;
}

int index_of_last_slash(char *dir_name)
{
    int last_slash = 0;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, dir_name, MAX_LINE_LENGTH);

    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            last_slash = i;
        }
    }

    return last_slash;
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

void append_directory(char *dir_name, char *filename)
{
    if (duplicate_directory(dir_name, filename) == 0)
    {
        FILE *file;

        if ((file = fopen(filename, "a")))
        {
            char new_line[MAX_LINE_LENGTH] = {0};
            strncpy(new_line, dir_name, MAX_LINE_LENGTH);

            new_line[strcspn(new_line, "\0")] = '\n';

            for (int i = 0; i < MAX_LINE_LENGTH; i++)
            {
                fputc(new_line[i], file);
            }

            fclose(file);
        }
    }
}

int mkdir(char *filename, char *id)
{
    if (strlen(id) <= MAX_LINE_LENGTH - 3)
    {
        if (id[0] != '/')
        {
            char dir_name[MAX_LINE_LENGTH] = {0};
            dir_name[0] = '=';
            strncpy(dir_name + 1, id, strlen(id));
            if (dir_name[strcspn(dir_name, "\0") - 1] != '/')
            {
                dir_name[strcspn(dir_name, "\0")] = '/';
            }

            if (duplicate_directory(dir_name, filename) == 0)
            {
            }
            else
            {
                fprintf(stderr, "The given directory already exists in the filesystem. Please try again!");
                exit(EXIT_FAILURE);
            }
            printf("Input id is: %s", dir_name);
        }
        else
        {
            fprintf(stderr, "Error! The directory name can not start with /. Please try again!");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "The given directory name exceeds the maximum length. Please try again!");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
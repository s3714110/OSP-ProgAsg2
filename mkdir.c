#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 255

int id_check(char *id)
{
    int valid = 0;

    if ((strcmp(id, ".") != 0) && (strcmp(id, "..") != 0) && (strcmp(id, "/") != 0))
    {
        valid = 1;
    }

    else
    {
        valid = 0;
    }

    return valid;
}
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

int index_of_next_slash(char *dir_name, int start_index)
{
    int next_slash = start_index;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, dir_name, MAX_LINE_LENGTH);

    for (int i = start_index + 1; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            next_slash = i;
            break;
        }
    }

    return next_slash;
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
    char output_dir[MAX_LINE_LENGTH] = {0};
    strncpy(output_dir, dir_name + 1, MAX_LINE_LENGTH);

    if (duplicate_directory(dir_name, filename) == 0)
    {

        FILE *file;

        if ((file = fopen(filename, "a")))
        {
            char new_line[MAX_LINE_LENGTH] = {0};
            strncpy(new_line, dir_name, MAX_LINE_LENGTH);

            new_line[strcspn(new_line, "\0")] = '\n';

            fputs(new_line, file);
            fclose(file);
            printf("Directory %s is created\n", output_dir);
        }
        else
        {
            fprintf(stderr, "Error! Can not access or create the note file. Please try again\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Directory %s already exists, no need to be automatically created\n", output_dir);
    }
}

int make_dir(char *filename, char *id)
{
    if (strlen(id) <= MAX_LINE_LENGTH - 3)
    {
        if (id_check(id) == 1)
        {
            char dir_name[MAX_LINE_LENGTH] = {0};
            dir_name[0] = '=';
            if (id[0] == '/')
            {
                strncpy(dir_name + 1, id + 1, strlen(id));
            }
            else
            {
                strncpy(dir_name + 1, id, strlen(id));
            }

            if (dir_name[strcspn(dir_name, "\0") - 1] != '/')
            {
                dir_name[strcspn(dir_name, "\0")] = '/';
            }

            if (duplicate_directory(dir_name, filename) == 0)
            {
                int number_of_slashes = count_slash(dir_name);
                int last_slash_index = 0;

                for (int i = 0; i < number_of_slashes; i++)
                {
                    int current_slash_index = index_of_next_slash(dir_name, last_slash_index);
                    char temp_dir[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_dir, dir_name, current_slash_index + 1);
                    append_directory(temp_dir, filename);

                    last_slash_index = current_slash_index;
                }
            }
            else
            {
                fprintf(stderr, "Error! The given directory already exists in the filesystem. Please try again!\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fprintf(stderr, "Error! There is a syntax error with given directory name. Please try again!\n");
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
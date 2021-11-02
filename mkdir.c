#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#define MAX_LINE_LENGTH 255

//counts number of slash in dir name, or ie, counts how many levels is the directory
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

//index of next slash starting from the left, to help extract the directories above
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

// checks to see if the directory is already in the FS or not
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
            temp_line[strcspn(temp_line, "\n")] = '\0'; // removes new line char so strcmp is accurate

            if (strcmp(dir_name, temp_line) == 0)
            {
                is_duplicate = 1;
            }
        }

        fclose(temp_file);
    }

    return is_duplicate;
}

// adds the directory into the end of FS file
void append_directory(char *dir_name, char *filename)
{
    char output_dir[MAX_LINE_LENGTH] = {0}; //this is mainly to display dirname for stdout without the first character
    strncpy(output_dir, dir_name + 1, MAX_LINE_LENGTH);

    //only if the directory does not exist yet in the FS file
    if (duplicate_directory(dir_name, filename) == 0)
    {

        FILE *file;

        if ((file = fopen(filename, "a")))
        {
            char new_line[MAX_LINE_LENGTH] = {0};
            strncpy(new_line, dir_name, MAX_LINE_LENGTH);

            new_line[strcspn(new_line, "\0")] = '\n'; // adds new line char at the end of string

            fputs(new_line, file);
            fclose(file);
            printf("Directory %s is created\n", output_dir);
        }
        else
        {
            fprintf(stderr, "Error! Can not access or modify the notes file. Please try again\n");
            exit(EX_NOINPUT);
        }
    }
    else // skips this functions if dir already exists
    {
        printf("Directory %s already exists, no need to be automatically created\n", output_dir);
    }
}

int make_dir(char *filename, char *id_name)
{
    if (strlen(id_name) <= MAX_LINE_LENGTH - 3) // - 3 to reserve space for FS file format characters like space, \n etc...
    {

        char dir_name[MAX_LINE_LENGTH] = {0};
        dir_name[0] = '=';  // sets first character for dir

        // if the provided dir name start with '/', remove it
        if (id_name[0] == '/')
        {
            strncpy(dir_name + 1, id_name + 1, strlen(id_name));
        }
        else
        {
            strncpy(dir_name + 1, id_name, strlen(id_name));
        }

        // if the provided dir name does not end with '/', add it
        if (dir_name[strcspn(dir_name, "\0") - 1] != '/')
        {
            dir_name[strcspn(dir_name, "\0")] = '/';
        }

        if (duplicate_directory(dir_name, filename) == 0)   // checks to see it the wanted directory already exists
        {
            int number_of_slashes = count_slash(dir_name);  // counts how many level is this dir
            int last_slash_index = 0;

            //recursively creates dir above the given directory as needed
            //new dir will be appened at the end of FS file
            for (int i = 0; i < number_of_slashes; i++)
            {
                int current_slash_index = index_of_next_slash(dir_name, last_slash_index);
                char temp_dir[MAX_LINE_LENGTH] = {0};
                strncpy(temp_dir, dir_name, current_slash_index + 1);
                append_directory(temp_dir, filename);

                last_slash_index = current_slash_index;
            }
        }
        else    // if yes, error
        {
            fprintf(stderr, "Error! The given directory already exists in the filesystem. Please try again!\n");
            exit(EX_DATAERR);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given directory name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }
    return EXIT_SUCCESS;
}
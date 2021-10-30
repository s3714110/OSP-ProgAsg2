#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define ValidFileCheck "NOTES V1.0"
#define MAX_LINE_LENGTH 255

int duplicate_name(char *name, int start_line, char *filename)
{
    int is_duplicate = 0;
    int line = start_line;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((temp_file = fopen(filename, "r")))
    {
        for (int i = 1; i <= line; i++)
        {
            fgets(temp_line, MAX_LINE_LENGTH, temp_file);
        }

        while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
        {
            line++;
            temp_line[strcspn(temp_line, "\n")] = '\0';

            if (strcmp(name, temp_line) == 0)
            {
                is_duplicate = 1;
                fprintf(stderr, "Duplicate names found at line %d and line %d !\n", start_line, line);
            }
        }

        fclose(temp_file);
    }

    return is_duplicate;
}

int directory_exist(char *name, char *filename)
{
    int exist = 0;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};
    int index_of_slash = -1;

    if (name[0] == '@')
    {
        for (int i = 0; i <= strlen(name); i++)
        {
            if (name[i] == '/')
            {
                index_of_slash = i;
            }
        }

        if (index_of_slash == -1)
        {
            exist = 1;
        }
        else
        {
            char dir_of_file[MAX_LINE_LENGTH] = {0};
            strncpy(dir_of_file, name, index_of_slash + 1);
            dir_of_file[0] = '=';

            if ((temp_file = fopen(filename, "r")))
            {
                while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
                {

                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    if (strcmp(dir_of_file, temp_line) == 0)
                    {
                        exist = 1;
                    }
                }

                fclose(temp_file);
            }
        }
    }

    else if (name[0] == '=')
    {
        char temp_path[MAX_LINE_LENGTH] = {0};
        strncpy(temp_path, name, MAX_LINE_LENGTH);
        temp_path[strcspn(temp_path, "\0") - 1] = '\0';

        for (int i = 0; i <= strlen(temp_path); i++)
        {
            if (temp_path[i] == '/')
            {
                index_of_slash = i;
            }
        }

        if (index_of_slash == -1)
        {
            exist = 1;
        }
        else
        {
            char dir_of_file[MAX_LINE_LENGTH] = {0};
            strncpy(dir_of_file, temp_path, index_of_slash + 1);
            

            if ((temp_file = fopen(filename, "r")))
            {
                while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
                {

                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    if (strcmp(dir_of_file, temp_line) == 0)
                    {
                        exist = 1;
                    }
                }

                fclose(temp_file);
            }
        }
    }
    return exist;
}

int init(char *filename)
{
    FILE *file;

    if ((file = fopen(filename, "r")))
    {
        printf("The notes file has been detected\n");
        int line_count = 0;

        char test_line[MAX_LINE_LENGTH] = ValidFileCheck;
        char first_line[MAX_LINE_LENGTH] = {0};
        char next_line[MAX_LINE_LENGTH] = {0};
        char previous_line[MAX_LINE_LENGTH] = {0};

        bool reading_a_file = false;

        fgets(first_line, MAX_LINE_LENGTH, file);
        line_count++;

        first_line[strcspn(first_line, "\n")] = '\0';

        if (strcmp(first_line, test_line) == 0)
        {
            printf("Note file identifier detected. Checking for file validity...\n");
            while (fgets(next_line, MAX_LINE_LENGTH, file))
            {
                line_count++;
                next_line[strcspn(next_line, "\n")] = '\0';

                if (next_line[0] == '@')
                {
                    if (duplicate_name(next_line, line_count, filename) == 1)
                    {
                        fprintf(stderr, "This is not a valid notes file. Please remove the notes file and run the program again\n");
                        exit(EXIT_FAILURE);
                    }

                    if (next_line[strcspn(next_line, "\0") - 1] == '/')
                    {
                        fprintf(stderr, "File name syntax error detected at line %d ! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EXIT_FAILURE);
                    }

                    if (directory_exist(next_line, filename) == 0)
                    {
                        fprintf(stderr, "%s is not a valid path because the directory does not exist ! This is not a valid notes file. Please remove the notes file and run the program again\n", next_line);
                        exit(EXIT_FAILURE);
                    }

                    reading_a_file = false;
                }
                else if (next_line[0] == '=')
                {
                    if (duplicate_name(next_line, line_count, filename) == 1)
                    {
                        fprintf(stderr, "This is not a valid notes file. Please remove the notes file and run the program again\n");
                        exit(EXIT_FAILURE);
                    }

                    if (next_line[strcspn(next_line, "\0") - 1] != '/')
                    {
                        fprintf(stderr, "Directory name syntax error detected at line %d ! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EXIT_FAILURE);
                    }

                    if (directory_exist(next_line, filename) == 0)
                    {
                        fprintf(stderr, "%s is not a valid path because the directory above this does not exist ! This is not a valid notes file. Please remove the notes file and run the program again\n", next_line);
                        exit(EXIT_FAILURE);
                    }

                    reading_a_file = false;
                }
                else if (next_line[0] == '#')
                {
                    reading_a_file = false;
                }
                else if (next_line[0] == ' ')
                {
                    if ( (previous_line[0] != '@') && (!reading_a_file) )
                    {
                        fprintf(stderr, "Error Detected at line %d! File content needs to be under a file! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EXIT_FAILURE);
                    }
                    reading_a_file = true;
                }
                else
                {
                    fprintf(stderr, "Unrecognised first character detected at line %d! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                    exit(EXIT_FAILURE);
                }
                strncpy(previous_line, next_line, MAX_LINE_LENGTH);
            }
        }
        else
        {
            fprintf(stderr, "Notes indentifier not found! This is not a valid notes file. Please remove the notes file and run the program again\n");
            exit(EXIT_FAILURE);
        }

        printf("Checking completed! This notes file is valid\n");
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

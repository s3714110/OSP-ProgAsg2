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
    bool reading_a_file = false;

    if (strlen(if_name) <= MAX_LINE_LENGTH - 3)
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
            FILE *file;
            FILE *buffer_file;
            char next_line[MAX_LINE_LENGTH] = {0};

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
            {
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    if (reading_a_file == true)
                    {
                        if (temp_line[0] == ' ')
                        {
                            temp_line[0] = '#';
                        }
                        else
                        {
                            reading_a_file = false;
                        }
                    }

                    if (strcmp(internal_file_name, temp_line) == 0)
                    {
                        temp_line[0] = '#';
                        reading_a_file = true;
                    }

                    
                    fputs(temp_line, buffer_file);
                    fputc('\n', buffer_file);
                }
                fclose(file);
                fclose(buffer_file);
                if (remove(filename) == 0)
                {
                    if (rename(BufferFileName, filename) == 0)
                    {
                        printf("File %s and its content have been marked as deleted for filesystem %s.\n", if_name, filename);
                    }
                    else
                    {
                        fprintf(stderr, "Error! Can not make changes to the buffer file. Please try again\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    fprintf(stderr, "Error! Can not make changes to the notes file. Please try again\n");
                    exit(EXIT_FAILURE);
                }
                
            }
            else
            {
                fprintf(stderr, "Error! Can not access or modify the notes or buffer file. Please try again\n");
                exit(EXIT_FAILURE);
            }
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
    bool reading_a_file = false;

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

        if (check_exist(internal_dir_name, filename) == 1)
        {
            FILE *file;
            FILE *buffer_file;
            char next_line[MAX_LINE_LENGTH] = {0};

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
            {
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    if (reading_a_file == true)
                    {
                        if (temp_line[0] == ' ')
                        {
                            temp_line[0] = '#';
                        }
                        else
                        {
                            reading_a_file = false;
                        }
                    }

                    if (temp_line[0] == '@')
                    {
                        if (strncmp(internal_dir_name + 1, temp_line + 1, strlen(internal_dir_name + 1)) == 0)
                        {
                            temp_line[0] = '#';
                            reading_a_file = true;
                        }
                    }
                    
                    if (strncmp(internal_dir_name, temp_line, strlen(internal_dir_name)) == 0)
                    {
                        temp_line[0] = '#';
                        
                    }

                    fputs(temp_line, buffer_file);
                    fputc('\n', buffer_file);
                }
                fclose(file);
                fclose(buffer_file);
                if (remove(filename) == 0)
                {
                    if (rename(BufferFileName, filename) == 0)
                    {
                        printf("Directory %s and all of its files and subdirectories have been marked as deleted for filesystem %s.\n", id_name, filename);
                    }
                    else
                    {
                        fprintf(stderr, "Error! Can not make changes to the buffer file. Please try again\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    fprintf(stderr, "Error! Can not make changes to the notes file. Please try again\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                fprintf(stderr, "Error! Can not access or modify the notes or buffer file. Please try again\n");
                exit(EXIT_FAILURE);
            }


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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#define BufferFileName "temp_buffer.notes" //buffer file that stores all changes and replace main FS file when done
#define MAX_LINE_LENGTH 255

//checks to see if a file or dir already exists
int check_exist(char *name, char *filename)
{
    int exists = 0;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((temp_file = fopen(filename, "r")))
    {
        //goes through FS file line by line
        while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0'; //remove new line char to compare strings

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
    bool reading_a_file = false; //tells the program when it can read a file

    //formats file name
    if (strlen(if_name) <= MAX_LINE_LENGTH - 3) // - 3 to reserve space for FS file format characters like space, \n etc...
    {
        char internal_file_name[MAX_LINE_LENGTH] = {0};
        internal_file_name[0] = '@'; // sets file character

        if (if_name[0] == '/') // in case the provided file starts with /, which is not valid for this FS format, this will remove it
        {
            strncpy(internal_file_name + 1, if_name + 1, strlen(if_name));
        }
        else
        {
            strncpy(internal_file_name + 1, if_name, strlen(if_name));
        }

        // checks to see if the file  exists
        if (check_exist(internal_file_name, filename) == 1)
        {
            FILE *file;
            FILE *buffer_file;
            char next_line[MAX_LINE_LENGTH] = {0};

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+"))) // opens FS file in read mode, and open buffer file in overwrite mode
            {
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0'; //removes new line char to compare string

                    if (reading_a_file == true) //reads file contents
                    {
                        if (temp_line[0] == ' ')
                        {
                            // marks every line in file content as #
                            temp_line[0] = '#';
                        }
                        else //stops reading when next line is not a file content
                        {
                            reading_a_file = false;
                        }
                    }

                    if (strcmp(internal_file_name, temp_line) == 0) //if the file name matches
                    {
                        //marks the first character as #
                        temp_line[0] = '#';
                        reading_a_file = true; //signals to start reading file content
                    }

                    //writes out to buffer file
                    fputs(temp_line, buffer_file);
                    fputc('\n', buffer_file);
                }
                fclose(file);
                fclose(buffer_file);

                //remove current FS name and rename buffer file to that FS name
                if (remove(filename) == 0)
                {
                    if (rename(BufferFileName, filename) == 0)
                    {
                        printf("File %s and its content have been marked as deleted for filesystem %s.\n", if_name, filename);
                    }
                    else
                    {
                        fprintf(stderr, "Error! Can not make changes to the buffer file. Please try again\n");
                        exit(EX_IOERR);
                    }
                }
                else
                {
                    fprintf(stderr, "Error! Can not make changes to the notes file. Please try again\n");
                    exit(EX_IOERR);
                }
            }
            else
            {
                fprintf(stderr, "Error! Can not access or modify the notes or buffer file. Please try again\n");
                exit(EX_NOINPUT);
            }
        }
        else // if not, errors
        {
            fprintf(stderr, "Error! The given file name can not be found in the filesystem. Please try again!\n");
            exit(EX_UNAVAILABLE);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given file name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }

    return EXIT_SUCCESS;
}

int remove_dir(char *filename, char *id_name)
{
    bool reading_a_file = false; //tells the program when it can read a file

    if (strlen(id_name) <= MAX_LINE_LENGTH - 3) // - 3 to reserve space for FS file format characters like space, \n etc...
    {
        char internal_dir_name[MAX_LINE_LENGTH] = {0};
        internal_dir_name[0] = '='; // sets first character for dir

        // if the provided dir name start with '/', remove it
        if (id_name[0] == '/')
        {
            strncpy(internal_dir_name + 1, id_name + 1, strlen(id_name));
        }
        else
        {
            strncpy(internal_dir_name + 1, id_name, strlen(id_name));
        }

        // if the provided dir name does not end with '/', add it
        if (internal_dir_name[strcspn(internal_dir_name, "\0") - 1] != '/')
        {
            internal_dir_name[strcspn(internal_dir_name, "\0")] = '/';
        }

        //only runs this function if the directory exists in FS
        if (check_exist(internal_dir_name, filename) == 1)
        {
            FILE *file;
            FILE *buffer_file;
            char next_line[MAX_LINE_LENGTH] = {0};

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
            {
                //goes through FS line by line
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0'; //removes new char to compare strings

                    if (reading_a_file == true)
                    {
                        if (temp_line[0] == ' ')
                        {
                            // marks every line of file content as deleted
                            temp_line[0] = '#';
                        }
                        else //stops when the next line is not a file content
                        {
                            reading_a_file = false;
                        }
                    }

                    if (temp_line[0] == '@') //if next line is a file name
                    {
                        if (strncmp(internal_dir_name + 1, temp_line + 1, strlen(internal_dir_name + 1)) == 0) //compares partial string of the filename to see if it contains directory name
                        {
                            //if yes mark file name as deleted
                            temp_line[0] = '#';
                            reading_a_file = true; // starts reading file content
                        }
                    }

                    //if dir name matches line
                    if (strncmp(internal_dir_name, temp_line, strlen(internal_dir_name)) == 0)
                    {
                        //marks dir as deleted
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
                        exit(EX_IOERR);
                    }
                }
                else
                {
                    fprintf(stderr, "Error! Can not make changes to the notes file. Please try again\n");
                    exit(EX_IOERR);
                }
            }
            else
            {
                fprintf(stderr, "Error! Can not access or modify the notes or buffer file. Please try again\n");
                exit(EX_NOINPUT);
            }
        }
        else
        {
            fprintf(stderr, "Error! The given directory name can not be found in the filesystem. Please try again!\n");
            exit(EX_UNAVAILABLE);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given directory name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }

    return EXIT_SUCCESS;
}

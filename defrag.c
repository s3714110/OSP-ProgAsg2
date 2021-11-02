#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#define BufferFileName "temp_buffer.notes" // buffer file that stores modified content. it will replace the current filesystem once the function is complete
#define MAX_LINE_LENGTH 255

// counts how many '/' in the file or dir name
// to determines how many directories there are in that path
int slash_counter(char *name)
{
    int slashes = 0;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, name, MAX_LINE_LENGTH);

    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            slashes++;
        }
    }

    return slashes;
}

// checks to see if a directory has already been written to the buffer file
// to prevent multiple duplicate entries being written
int dir_exist_in_buffer(char *dir_name, char *buffer_filename)
{
    int is_exist = 0;
    FILE *buffer_temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((buffer_temp_file = fopen(buffer_filename, "r")))
    {
        while (fgets(temp_line, MAX_LINE_LENGTH, buffer_temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0';

            if (strcmp(dir_name, temp_line) == 0)
            {
                is_exist = 1;
            }
        }
        fclose(buffer_temp_file);
    }

    return is_exist;
}

// finds the index of the slash before the last one
// to help figure out the directory that contains a directory
int index_of_one_up_slash(char *dir_name)
{
    int index = 0;
    char temp_path[MAX_LINE_LENGTH] = {0};
    strncpy(temp_path, dir_name, MAX_LINE_LENGTH);
    temp_path[strlen(temp_path) - 1] = '\0';

    for (int i = 0; i < strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            index = i;
        }
    }

    return index;
}

// removes deleted entries
int defrag(char *filename)
{
    FILE *file;
    FILE *buffer_file;
    char next_line[MAX_LINE_LENGTH] = {0};

    if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+"))) //opens fs in read mode and opens buffer file in overwrite mode
    {
        // goes through fs line by line
        while (fgets(next_line, MAX_LINE_LENGTH, file))
        {
            char temp_line[MAX_LINE_LENGTH] = {0};
            strncpy(temp_line, next_line, MAX_LINE_LENGTH);
            temp_line[strcspn(temp_line, "\n")] = '\0'; // removed new line char that comes with fgets

            //only writes entries without the deleted character into buffer file
            if (temp_line[0] != '#')
            {
                fputs(temp_line, buffer_file);
                fputc('\n', buffer_file);
            }
        }
        fclose(file);
        fclose(buffer_file);

        //remove current FS file and rename the temp buffer file into that FS file
        if (remove(filename) == 0)
        {
            if (rename(BufferFileName, filename) == 0)
            {
                printf("Filesystem %s has been defragmented.\n", filename);
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
    return EXIT_SUCCESS;
}

// sorts files and directories based on number of slashes, or ie number of directories above it
// the one with least slash will be put first
// this is to later help with sorting into file tree sequence
void sort_by_slashes(char *filename)
{
    FILE *file;
    FILE *buffer_file;
    char next_line[MAX_LINE_LENGTH] = {0};
    bool reading_a_file = false;
    int number_of_slash = 0;
    bool searching = true;
    bool found = false;

    if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
    {

        fgets(next_line, MAX_LINE_LENGTH, file);
        fputs(next_line, buffer_file);
        fclose(file);

        // loop to keep going through FS file until all entries are found
        while (searching)
        {
            found = false;
            if ((file = fopen(filename, "r")))
            {
                fgets(next_line, MAX_LINE_LENGTH, file);
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    //writes directories with least number of slashes first into temp buffer file
                    if ((temp_line[0] == '='))
                    {
                        if (slash_counter(temp_line) == number_of_slash)
                        {
                            fputs(temp_line, buffer_file);
                            fputc('\n', buffer_file);
                            found = true;
                        }
                    }
                }
            }
            fclose(file);

            if ((file = fopen(filename, "r")))
            {
                fgets(next_line, MAX_LINE_LENGTH, file);
                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    //then writes files name with least number of slashes first and their content into temp buffer file

                    if (reading_a_file == true)
                    {
                        if (temp_line[0] == ' ')
                        {
                            fputs(temp_line, buffer_file);
                            fputc('\n', buffer_file);
                        }
                        else
                        {
                            reading_a_file = false;
                        }
                    }

                    if ((temp_line[0] == '@'))
                    {
                        if (slash_counter(temp_line) == number_of_slash)
                        {
                            fputs(temp_line, buffer_file);
                            fputc('\n', buffer_file);
                            reading_a_file = true;
                            found = true;
                        }
                    }
                }
            }
            fclose(file);

            // increase the number of slash by one at every loop
            number_of_slash++;

            // if no more entries are found, ends the loop
            if (!found)
            {
                searching = false;
            }
        }
        fclose(buffer_file);
        if (remove(filename) == 0)
        {
            if (rename(BufferFileName, filename) == 0)
            {
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

// this methods continues from the method above
// what this does is it sort the file recursively
// meaning for a directory, it has to finds all the subdirectries and the files first, then move on to the directory at the same level
// this is best demonstrated using the command ls -lR in a multi level directory
void sort_by_dirs_files(char *filename)
{
    FILE *file;
    FILE *buffer_file;
    char next_line[MAX_LINE_LENGTH] = {0};
    bool reading_a_file = false;
    bool searching = true;
    bool found = false; // to indicate if an entry is found

    char current_dir_name[MAX_LINE_LENGTH] = {0}; //used to store the current dir so that the next loop can rely on this to find out the sub dirs
    char blank_string[MAX_LINE_LENGTH] = {0};     // used to reset a char array

    int line_skip = 0;           // used to skip lines to move on to next entry so that the program won't have to read old entries again
    bool found_next_dir = false; // to indicate if theres any more directory left that has not been sorted

    if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
    {
        fgets(next_line, MAX_LINE_LENGTH, file);
        fputs(next_line, buffer_file);
        line_skip++;

        // files at root folder of FS do not need to be sorted so those are copied into the temp file first
        while (fgets(next_line, MAX_LINE_LENGTH, file))
        {
            if (next_line[0] == '=')
            {
                break;
            }
            fputs(next_line, buffer_file);
            line_skip++;
        }

        fclose(file);
        fclose(buffer_file);

        while (searching)
        {
            found = false;
            found_next_dir = false;

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "a"))) // opens buffer file at appened mode now because we do not want to overwrite it
            {
                // skips line as needed
                for (int i = 1; i <= line_skip; i++)
                {
                    fgets(next_line, MAX_LINE_LENGTH, file);
                }

                while (fgets(next_line, MAX_LINE_LENGTH, file))
                {
                    if (next_line[0] == '=')
                    {
                        found_next_dir = true;
                    }

                    char temp_line[MAX_LINE_LENGTH] = {0};
                    strncpy(temp_line, next_line, MAX_LINE_LENGTH);
                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    //copies out the file content
                    if (reading_a_file == true)
                    {
                        if (temp_line[0] == ' ')
                        {
                            fputs(temp_line, buffer_file);
                            fputc('\n', buffer_file);
                        }
                        else
                        {
                            reading_a_file = false;
                        }
                    }

                    //checks to see if a file belongs to the current dir
                    // if yes then set the reading_a_file to true to start writing out file content
                    if ((temp_line[0] == '@'))
                    {

                        if (strncmp(temp_line + 1, current_dir_name + 1, strlen(current_dir_name + 1)) == 0)
                        {
                            if (dir_exist_in_buffer(temp_line, BufferFileName) == 0)
                            {
                                fputs(temp_line, buffer_file);
                                fputc('\n', buffer_file);
                                reading_a_file = true;
                            }
                        }
                    }

                    if ((temp_line[0] == '='))
                    {
                        // if current dir name is blank then use the name of this current directory
                        if (strlen(current_dir_name) == 0)
                        {

                            strncpy(current_dir_name, temp_line, MAX_LINE_LENGTH);
                        }

                        //if not, search for subdirectories by comparing substrings
                        if (strncmp(current_dir_name, temp_line, strlen(current_dir_name)) == 0)
                        {
                            // if there's match then set current dir name to this current directory
                            // so that this will keep going recursively until no more entries are found
                            if (dir_exist_in_buffer(temp_line, BufferFileName) == 0)
                            {
                                strncpy(current_dir_name, temp_line, MAX_LINE_LENGTH);
                                fputs(temp_line, buffer_file);
                                fputc('\n', buffer_file);
                                found = true;
                            }
                        }
                    }
                }
            }
            fclose(file);
            fclose(buffer_file);

            // if no entries are found
            if (!found)
            {
                int one_up_slash_index = index_of_one_up_slash(current_dir_name);
                char temp_dir_name[MAX_LINE_LENGTH] = {0};
                if (one_up_slash_index > 0) //if there are more directories above, move current dir name up 1 level
                {
                    strncpy(temp_dir_name, current_dir_name, one_up_slash_index + 1);
                    strncpy(current_dir_name, temp_dir_name, MAX_LINE_LENGTH);
                }
                else
                {
                    // if not, then skip more lines
                    strncpy(current_dir_name, blank_string, MAX_LINE_LENGTH);
                    line_skip++;
                    if (!found_next_dir) // if no more uncopied entries are found, then ends loop
                    {
                        searching = false;
                    }
                }
            }
        }

        // removes current FS and renames temp file into that FS name
        if (remove(filename) == 0)
        {
            if (rename(BufferFileName, filename) == 0)
            {
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

int sort(char *filename)
{
    //sort by slashes first so that sort by dirs files doesnt need to run as many loops
    sort_by_slashes(filename);
    sort_by_dirs_files(filename);
    printf("Filesystem %s has been sorted into file tree sequence.\n", filename);
    return EXIT_SUCCESS;
}
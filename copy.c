#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#include "mkdir.h"
#include "rm.h"
#include "base64.h"

#define MAX_LINE_LENGTH 255
#define TempFileExtention ".temp" //for buffer file that is used to store new changes to the filesystem. The buffer file then replaces the original FS file when complete

// returns the value of last '/' in a file or dir name, to help get the path of directory that contains it
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

// plain text copyin
int copy_in(char *filename, char *ef_name, char *if_name)
{
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

        if (check_exist(internal_file_name, filename) == 1) // checks to see if the internal file already exists, if it does then marks old file as deleted
        {
            printf("File with name %s found on the filesystem %s. The program will now delete the old file ... \n", if_name, filename);
            remove_file(filename, if_name);
        }

        if (index_of_last_slash(if_name) > 0) // if the internal file is not at root, then checks for directory above it
        {
            // extracts dir name from file name
            char id_name[MAX_LINE_LENGTH] = {0};
            strncpy(id_name, if_name, index_of_last_slash(if_name) + 1);

            char internal_dir_name[MAX_LINE_LENGTH] = {0};
            internal_dir_name[0] = '='; // sets dir character
            if (id_name[0] == '/')      // in case the provided dir name starts with /, which is not valid for this FS format, this will remove it
            {
                strncpy(internal_dir_name + 1, id_name + 1, strlen(id_name));
            }
            else
            {
                strncpy(internal_dir_name + 1, id_name, strlen(id_name));
            }

            // if provided dir name does not end with /, this will add it to make it valid
            if (internal_dir_name[strcspn(internal_dir_name, "\0") - 1] != '/')
            {
                internal_dir_name[strcspn(internal_dir_name, "\0")] = '/';
            }

            // check to see if the directory exists in the FS, if not then call make_dir to create all immediate directories
            if (duplicate_directory(internal_dir_name, filename) == 0)
            {
                printf("Directory %s not found on the filesystem %s. The program will now automatically create directories as needed ... \n", id_name, filename);
                make_dir(filename, id_name);
            }
        }

        FILE *file;
        FILE *external_file;

        if ((file = fopen(filename, "a"))) // opens fs in append mode
        {
            if ((external_file = fopen(ef_name, "r"))) // opens external file in read mode
            {
                // inserts internal file name in FS
                fputs(internal_file_name, file);
                fputc('\n', file);

                // goes through external file line by line
                // and copies it into the FS
                char next_external_line[MAX_LINE_LENGTH - 3] = {0};
                while (fgets(next_external_line, MAX_LINE_LENGTH - 3, external_file))
                {
                    next_external_line[strcspn(next_external_line, "\n")] = '\0'; // removes newline char in external line if needed
                    char next_internal_line[MAX_LINE_LENGTH] = {0};
                    next_internal_line[0] = ' '; // add space character for file content

                    strncpy(next_internal_line + 1, next_external_line, strlen(next_external_line));

                    fputs(next_internal_line, file);
                    fputc('\n', file);
                }

                fclose(external_file);
            }
            else
            {
                fprintf(stderr, "Error! Can not find or access external file. Please try again\n");
                exit(EX_NOINPUT);
            }

            fclose(file);
            printf("Content of external file %s has been copied into internal file %s on the filesystem %s\n", ef_name, if_name, filename);
        }
        else
        {
            fprintf(stderr, "Error! Can not access notes file. Please try again\n");
            exit(EX_NOINPUT);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given internal file name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }
    return EXIT_SUCCESS;
}

//plain text copyout
int copy_out(char *filename, char *if_name, char *ef_name)
{
    if (strlen(if_name) <= MAX_LINE_LENGTH - 3) // - 3 to reserve space for FS file format characters like space, \n etc...
    {
        char internal_file_name[MAX_LINE_LENGTH] = {0};
        internal_file_name[0] = '@'; // sets file character

        // in case the provided file starts with /, which is not valid for this FS format, this will remove it
        if (if_name[0] == '/')
        {
            strncpy(internal_file_name + 1, if_name + 1, strlen(if_name));
        }
        else
        {
            strncpy(internal_file_name + 1, if_name, strlen(if_name));
        }

        // checks to see if the internal file exist in the FS, if not, outputs error and exits
        if (check_exist(internal_file_name, filename) == 0)
        {
            fprintf(stderr, "Error! The given internal file can not be found on filesystem %s . Please try again!\n", filename);
            exit(EX_UNAVAILABLE);
        }

        FILE *file;
        FILE *external_file;
        bool reading_a_file = false; // checks to see if the program is currently reading a file content

        if ((file = fopen(filename, "r"))) //opens fs in read mode
        {
            if ((external_file = fopen(ef_name, "w+"))) //opens external file in overwrite mode, so any old data of external file will be deleted
            {
                char next_internal_line[MAX_LINE_LENGTH] = {0};

                // goes through FS file line by line
                while (fgets(next_internal_line, MAX_LINE_LENGTH, file))
                {
                    next_internal_line[strcspn(next_internal_line, "\n")] = '\0';

                    // if reading a file is true and next line is a file content, writes it out to external file
                    if (reading_a_file == true)
                    {
                        if (next_internal_line[0] == ' ')
                        {
                            char next_external_line[MAX_LINE_LENGTH - 3] = {0};
                            strncpy(next_external_line, next_internal_line + 1, strlen(next_internal_line));
                            fputs(next_external_line, external_file);
                            fputc('\n', external_file);
                        }
                        else // stops writing if next line is not a file content
                        {
                            reading_a_file = false;
                        }
                    }

                    // if the current line is a file and matches the provide filename
                    // then marks reading_a_file
                    if (next_internal_line[0] == '@')
                    {
                        if (strcmp(next_internal_line, internal_file_name) == 0)
                        {
                            reading_a_file = true;
                        }
                    }
                }

                fclose(external_file);
            }
            else
            {
                fprintf(stderr, "Error! Can not create or modify external file. Please try again\n");
                exit(EX_CANTCREAT);
            }

            fclose(file);
            printf("Content of internal file %s on the filesystem %s has been copied out to externel file %s \n", if_name, filename, ef_name);
        }
        else
        {
            fprintf(stderr, "Error! Can not access notes file. Please try again\n");
            exit(EX_NOINPUT);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given internal file name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }

    return EXIT_SUCCESS;
}

// base64 encoding for copyin
// most of file and dirctory names processing part are similar to the plain ones
int copy_in_b64(char *filename, char *ef_name, char *if_name)
{
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
            if ((external_file = fopen(ef_name, "rb"))) // opens external file in reading binary mode
            {
                fputs(internal_file_name, file);
                fputc('\n', file);

                unsigned char *file_to_string = 0; // uses unsigned char array to store binary characters
                long length;                       // length of external file

                // sets seaker to the end then run ftell to get how many character there are in the file
                // then sets it back to the beginning to prepare for read
                fseek(external_file, 0, SEEK_END);
                length = ftell(external_file);
                fseek(external_file, 0, SEEK_SET);

                //allocates character array using the returned length of file
                file_to_string = malloc(length);

                if (file_to_string) //reads content of the file and puts them into the array
                {
                    fread(file_to_string, 1, length, external_file);
                }

                char *encoded_data;
                encoded_data = b64_encode(file_to_string, length); //encodes the char array into base64

                // because the maximum length of a record is 255, minus 3 reserved space, for file content
                // to figure out how many lines the encoded data will take
                // get the length of encoded data, divide by maximum allowed number of space, then plus 1 for leftover characters
                int encoded_lines = (strlen(encoded_data) / (MAX_LINE_LENGTH - 3)) + 1;

                // copies the encoded data line by line
                for (int i = 0; i < encoded_lines; i++)
                {
                    char next_internal_line[MAX_LINE_LENGTH] = {0};
                    next_internal_line[0] = ' '; // sets the file content characters

                    // at every for loop, move the pointer of encoded array by the maximun allowed space
                    strncpy(next_internal_line + 1, encoded_data + i * (MAX_LINE_LENGTH - 3), MAX_LINE_LENGTH - 3);

                    fputs(next_internal_line, file);
                    fputc('\n', file);
                }

                fclose(external_file);
                free(file_to_string);
            }
            else
            {
                fprintf(stderr, "Error! Can not find or access external file. Please try again\n");
                exit(EX_NOINPUT);
            }

            fclose(file);
            printf("Content of external file %s has been encoded and copied into internal file %s on the filesystem %s\n", ef_name, if_name, filename);
        }
        else
        {
            fprintf(stderr, "Error! Can not access notes file. Please try again\n");
            exit(EX_NOINPUT);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given internal file name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }
    return EXIT_SUCCESS;
}

// base64 decoding for copyout
// most of file and dirctory names processing part are similar to the plain ones
int copy_out_b64(char *filename, char *if_name, char *ef_name)
{
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

        if (check_exist(internal_file_name, filename) == 0)
        {
            fprintf(stderr, "Error! The given internal file can not be found on filesystem %s . Please try again!\n", filename);
            exit(EX_UNAVAILABLE);
        }

        char temp_external_filename[MAX_LINE_LENGTH] = {0};
        strncpy(temp_external_filename, ef_name, strlen(ef_name));
        strcat(temp_external_filename, TempFileExtention);

        FILE *file;
        FILE *temp_external_file; // this file is used to store the encoded data before it is decoded, and will be deleted when this method completes
        bool reading_a_file = false;

        if ((file = fopen(filename, "r")))
        {
            if ((temp_external_file = fopen(temp_external_filename, "w+"))) // opens the tempfile in weite
            {
                char next_internal_line[MAX_LINE_LENGTH] = {0};

                // finds the file that needs to be copied out in the FS
                // then copies the encoded data to the tempfile
                while (fgets(next_internal_line, MAX_LINE_LENGTH, file))
                {
                    next_internal_line[strcspn(next_internal_line, "\n")] = '\0';

                    if (reading_a_file == true)
                    {
                        if (next_internal_line[0] == ' ')
                        {
                            char next_external_line[MAX_LINE_LENGTH - 3] = {0};
                            strncpy(next_external_line, next_internal_line + 1, strlen(next_internal_line));
                            fputs(next_external_line, temp_external_file);
                        }
                        else
                        {
                            reading_a_file = false;
                        }
                    }

                    if (next_internal_line[0] == '@')
                    {
                        if (strcmp(next_internal_line, internal_file_name) == 0)
                        {
                            reading_a_file = true;
                        }
                    }
                }

                fclose(temp_external_file);

                if ((temp_external_file = fopen(temp_external_filename, "r"))) // opens tempfile in read mode
                {

                    char *file_to_string = 0;
                    long length;

                    // copies the content of tempfile into a char array
                    fseek(temp_external_file, 0, SEEK_END);
                    length = ftell(temp_external_file);
                    fseek(temp_external_file, 0, SEEK_SET);
                    file_to_string = malloc(length);
                    if (file_to_string)
                    {
                        fread(file_to_string, 1, length, temp_external_file);
                    }

                    fclose(temp_external_file);

                    size_t decoded_length;
                    unsigned char *decoded_data;

                    // finds out how much space does the decoded data will take
                    // plus 1 is for the null terminator
                    decoded_length = b64_decoded_size(file_to_string) ;
                    decoded_data = malloc(decoded_length);

                    //decodes the char array, which contains the encoded file content
                    //and copies it into the new unsigned char array
                    if (!b64_decode(file_to_string, decoded_data, decoded_length))
                    {
                        fprintf(stderr, "Error! Something went wront with base64 decoding. Please try again\n");
                        exit(EX_SOFTWARE);
                    }

                    

                    FILE *external_file;
                    if ((external_file = fopen(ef_name, "w+b"))) // opens the external file in write binary mode
                    {
                        fwrite(decoded_data, 1, decoded_length, external_file); //  writes the decoded data out to external file
                        fclose(external_file);
                    }

                    else
                    {
                        fprintf(stderr, "Error! Can not create or modify external file. Please try again\n");
                        exit(EX_CANTCREAT);
                    }

                    free(file_to_string);
                    free(decoded_data);
                }

                else
                {
                    fprintf(stderr, "Error! Can not access temp file. Please try again\n");
                    exit(EX_NOINPUT);
                }

                if (remove(temp_external_filename) != 0)
                {

                    fprintf(stderr, "Error! Can not make changes to the temp file. Please try again\n");
                    exit(EX_IOERR);
                }
            }
            else
            {
                fprintf(stderr, "Error! Can not create or modify temp file. Please try again\n");
                exit(EX_CANTCREAT);
            }

            fclose(file);
            printf("Encoded content of internal file %s on the filesystem %s has been decoded and copied out to externel file %s \n", if_name, filename, ef_name);
        }
        else
        {
            fprintf(stderr, "Error! Can not access notes file. Please try again\n");
            exit(EX_NOINPUT);
        }
    }
    else
    {
        fprintf(stderr, "Error! The given internal file name exceeds the maximum length. Please try again!\n");
        exit(EX_DATAERR);
    }
    return EXIT_SUCCESS;
}
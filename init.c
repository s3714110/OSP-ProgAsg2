#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#define ValidFileCheck "NOTES V1.0"
#define MAX_LINE_LENGTH 255

//checks to see if there are 2 or more file or dir names in the FS file
int duplicate_name(char *name, int start_line, char *filename)
{
    int is_duplicate = 0;
    int line = start_line; //indicate which line this functions starts searching from to avoid reading the same entry twice
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};

    if ((temp_file = fopen(filename, "r")))
    {
        // skipping lines to get to the start line
        for (int i = 1; i <= line; i++)
        {
            fgets(temp_line, MAX_LINE_LENGTH, temp_file);
        }

        while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
        {
            line++;
            temp_line[strcspn(temp_line, "\n")] = '\0'; //removes new line char that comes with fgets so strcmp will be accurate

            if (strcmp(name, temp_line) == 0) //if finds a name the same as the given name
            {
                is_duplicate = 1; // marks as duplicate
                fprintf(stderr, "Error! Duplicate names found at line %d and line %d !\n", start_line, line);
            }
        }

        fclose(temp_file);
    }

    return is_duplicate;
}

//checks to see if there exists directory above the directory that a file or dir is in
//for example, if theres an entry called @dir1/dir2/dir3/file, dir1 dir2 and dir3 must exist in the FS file
int directory_exist(char *name, char *filename)
{
    int exist = 0;
    FILE *temp_file;
    char temp_line[MAX_LINE_LENGTH] = {0};
    int index_of_slash = -1;

    // if its a file
    if (name[0] == '@')
    {
        //finds if theres any directories for this file
        for (int i = 0; i <= strlen(name); i++)
        {
            if (name[i] == '/')
            {
                index_of_slash = i;
            }
        }

        //if not, skips this function
        if (index_of_slash == -1)
        {
            exist = 1;
        }
        // if yes
        else
        {
            char dir_of_file[MAX_LINE_LENGTH] = {0};
            strncpy(dir_of_file, name, index_of_slash + 1); //extracts dir name from the file name
            dir_of_file[0] = '=';                           //change first character to = to compare with directories

            if ((temp_file = fopen(filename, "r")))
            {
                while (fgets(temp_line, MAX_LINE_LENGTH, temp_file))
                {

                    temp_line[strcspn(temp_line, "\n")] = '\0';

                    if (strcmp(dir_of_file, temp_line) == 0) //if dir name is found, then marks as exist
                    {
                        exist = 1;
                    }
                }

                fclose(temp_file);
            }
        }
    }

    // if its a dir
    // then do the same as above but without the need to change the first charcter to '=' because its already a directory
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

    if ((file = fopen(filename, "r"))) // checks to see if the FS file exists
    {
        printf("The notes file has been detected\n");
        int line_count = 0; //counts the line so if theres an error, can outputs what line the errors from the FS comes from

        char test_line[MAX_LINE_LENGTH] = ValidFileCheck;
        char first_line[MAX_LINE_LENGTH] = {0};    // store the first line read
        char next_line[MAX_LINE_LENGTH] = {0};     // stores the next line read
        char previous_line[MAX_LINE_LENGTH] = {0}; // stores the previous line read

        bool reading_a_file = false; // tells the program when it can reads the content of a file, so in case a file content does not start under a file name, an error can be detected

        fgets(first_line, MAX_LINE_LENGTH, file);
        line_count++;

        first_line[strcspn(first_line, "\n")] = '\0';

        if (strcmp(first_line, test_line) == 0) // checks if the FS file has the correct header
        {
            printf("Notes file identifier detected. Checking for file validity...\n");
            while (fgets(next_line, MAX_LINE_LENGTH, file))
            {
                line_count++;
                next_line[strcspn(next_line, "\n")] = '\0';

                if (next_line[0] == '@') // if next line is a filename
                {
                    if (duplicate_name(next_line, line_count, filename) == 1) // checks file duplicate
                    {
                        fprintf(stderr, "Error! This is not a valid notes file. Please remove the notes file and run the program again\n");
                        exit(EX_DATAERR);
                    }

                    if (next_line[strcspn(next_line, "\0") - 1] == '/') // file name can not end with '/'
                    {
                        fprintf(stderr, "Error! File name syntax error detected at line %d ! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EX_DATAERR);
                    }

                    if (directory_exist(next_line, filename) == 0) // checks to see if the directory the file is in exists
                    {
                        fprintf(stderr, "Error! %s is not a valid path because the directory does not exist ! This is not a valid notes file. Please remove the notes file and run the program again\n", next_line);
                        exit(EX_DATAERR);
                    }

                    reading_a_file = false;
                }
                else if (next_line[0] == '=') // if next line is directory name
                {
                    if (duplicate_name(next_line, line_count, filename) == 1) // check directory duplicate
                    {
                        fprintf(stderr, "Error! This is not a valid notes file. Please remove the notes file and run the program again\n");
                        exit(EX_DATAERR);
                    }

                    if (next_line[strcspn(next_line, "\0") - 1] != '/') // dir name has to end with '/'
                    {
                        fprintf(stderr, "Error! Directory name syntax error detected at line %d ! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EX_DATAERR);
                    }

                    if (directory_exist(next_line, filename) == 0) // checks to see if the directories above this directory (if there is any) exists
                    {
                        fprintf(stderr, "Error! %s is not a valid path because the directory above this does not exist ! This is not a valid notes file. Please remove the notes file and run the program again\n", next_line);
                        exit(EX_DATAERR);
                    }

                    reading_a_file = false;
                }
                else if (next_line[0] == '#') // if next line is deleted entries, do nothing
                {
                    reading_a_file = false;
                }
                else if (next_line[0] == ' ') // if next line is file content
                {
                    // the previous line before the file starts has to be a file name
                    // and reading a file is true
                    if ((previous_line[0] != '@') && (!reading_a_file))
                    {
                        fprintf(stderr, "Error Detected at line %d! File content needs to be under a file! This is not a valid notes file. Please remove the notes file and run the program again\n", line_count);
                        exit(EX_DATAERR);
                    }
                    reading_a_file = true; // this is true until the next line is not a file content
                }
                else // errors if first character is not any one of the above
                {
                    fprintf(stderr, "Error! Unrecognised first character %c detected at line %d! This is not a valid notes file. Please remove the notes file and run the program again\n", next_line[0], line_count);
                    exit(EX_DATAERR);
                }
                strncpy(previous_line, next_line, MAX_LINE_LENGTH);
            }
        }
        else
        {
            fprintf(stderr, "Error! Notes identifier not found! This is not a valid notes file. Please remove the notes file and run the program again\n");
            exit(EX_DATAERR);
        }

        printf("Checking completed! This notes file is valid\n");
        fclose(file);
    }
    else //if not, tries to create a new one
    {
        printf("Notes file not detected. Creating a new notes files...\n");

        if ((file = fopen(filename, "w"))) //creates a new notes file
        {
            // inserts header to make the FS file valid
            fprintf(file, "%s\n", ValidFileCheck);
            fclose(file);
            printf("Notes file %s created.\n", filename);
        }
        else
        {
            fprintf(stderr, "Error! Can not access or create the notes file. Please try again\n");
            exit(EX_CANTCREAT);
        }
    }

    return EXIT_SUCCESS;
}

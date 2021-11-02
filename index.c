#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

//index file has the format:
//first line: file name or dir name with same leading characters as in FS file
//second line: the line of that file or dir in the FS file with a space leading character
#define IndexFileExtension ".index"
#define MAX_LINE_LENGTH 255

int create_index(char *filename)
{
    char index_filename[MAX_LINE_LENGTH] = {0};
    strncpy(index_filename, filename, strlen(filename));

    strcat(index_filename, IndexFileExtension); //concats file extention to create name for the index file

    FILE *file;
    FILE *index_file;
    int line_count = 0; // counts the line

    if ((file = fopen(filename, "r")) && (index_file = fopen(index_filename, "w+")))
    {
        char next_line[MAX_LINE_LENGTH] = {0};
        // goes throught FS file line by line
        while (fgets(next_line, MAX_LINE_LENGTH, file))
        {
            char next_index_line[MAX_LINE_LENGTH] = {0};
            line_count++;
            if (next_line[0] == '@')
            {
                char line_count_str[MAX_LINE_LENGTH];
                sprintf(line_count_str, "%d", line_count); // converts int to char array

                // writes file name and its line number into index file
                fputs(next_line, index_file);
                next_index_line[0] = ' ';
                strcat(next_index_line, line_count_str);
                fputs(next_index_line, index_file);
                fputc('\n', index_file);
            }
            else if (next_line[0] == '=')
            {
                char line_count_str[MAX_LINE_LENGTH];
                sprintf(line_count_str, "%d", line_count); // converts int to char array

                // writes dir name and its line number into index file
                fputs(next_line, index_file);
                next_index_line[0] = ' ';
                strcat(next_index_line, line_count_str);
                fputs(next_index_line, index_file);
                fputc('\n', index_file);
            }
        }

        fclose(file);
        fclose(index_file);
        printf("Index file %s created.\n", index_filename);
    }
    else
    {
        fprintf(stderr, "Error! Can not read or access files. Please try again\n");
        exit(EX_NOINPUT);
    }
    return EXIT_SUCCESS;
}

// lists out the index file to stdout
int read_index(char *filename)
{
    char index_filename[MAX_LINE_LENGTH] = {0};
    strncpy(index_filename, filename, strlen(filename));
    strcat(index_filename, IndexFileExtension);

    FILE *index_file;
    if ((index_file = fopen(index_filename, "r")))
    {
        printf("Reading index file %s ...\n\n", index_filename);
        char next_line[MAX_LINE_LENGTH] = {0};
        while (fgets(next_line, MAX_LINE_LENGTH, index_file))
        {
            next_line[strcspn(next_line, "\n")] = '\0';
            char entry_name[MAX_LINE_LENGTH] = {0};
            strncpy(entry_name, next_line, MAX_LINE_LENGTH);

            if (next_line[0] == '@')
            {
                fgets(next_line, MAX_LINE_LENGTH, index_file);
                next_line[strcspn(next_line, "\n")] = '\0';

                printf("File %s starts at line %s\n", entry_name + 1, next_line + 1);
            }
            else if (next_line[0] == '=')
            {
                fgets(next_line, MAX_LINE_LENGTH, index_file);
                next_line[strcspn(next_line, "\n")] = '\0';

                printf("Directory %s starts at line %s\n", entry_name + 1, next_line + 1);
            }
        }

        fclose(index_file);
    }
    else
    {
        fprintf(stderr, "Error! Can not read or access index files. Please try again\n");
        exit(EX_NOINPUT);
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define IndexFileExtension ".index"
#define MAX_LINE_LENGTH 255


int create_index(char *filename)
{
    char index_filename[MAX_LINE_LENGTH] = {0};
    strncpy(index_filename, filename, strlen(filename));

    strcat(index_filename, IndexFileExtension);
    
    FILE *file;
    FILE *index_file;
    int line_count = 0;

    if ((file = fopen(filename, "r")) && (index_file = fopen(index_filename, "w+")))
    {
        char next_line[MAX_LINE_LENGTH] = {0};
        while (fgets(next_line, MAX_LINE_LENGTH, file))
        {
            char next_index_line[MAX_LINE_LENGTH] = {0};
            line_count++;
            if (next_line[0] == '@')
            {
                char line_count_str[MAX_LINE_LENGTH];
                sprintf(line_count_str, "%d", line_count);

                fputs(next_line, index_file);
                next_index_line[0] = ' ';
                strcat(next_index_line, line_count_str);
                fputs(next_index_line, index_file);
                fputc('\n', index_file);
            }
            else if (next_line[0] == '=')
            {
                char line_count_str[MAX_LINE_LENGTH];
                sprintf(line_count_str, "%d", line_count);

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
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

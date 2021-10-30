#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define BufferFileName "temp_buffer.notes"
#define MAX_LINE_LENGTH 255

int defrag(char *filename)
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

            if (temp_line[0] != '#')
            {
                fputs(temp_line, buffer_file);
                fputc('\n', buffer_file);
            }
        }
        fclose(file);
        fclose(buffer_file);
        if (remove(filename) == 0)
        {
            if (rename(BufferFileName, filename) == 0)
            {
                printf("Filesystem %s has been defragmented.\n", filename);
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
    return EXIT_SUCCESS;
}
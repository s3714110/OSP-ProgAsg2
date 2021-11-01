#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>

#define BufferFileName "temp_buffer.notes"
#define MAX_LINE_LENGTH 255

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

            number_of_slash++;

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

void sort_by_dirs_files(char *filename)
{
    FILE *file;
    FILE *buffer_file;
    char next_line[MAX_LINE_LENGTH] = {0};
    bool reading_a_file = false;
    bool searching = true;
    bool found = false;

    char current_dir_name[MAX_LINE_LENGTH] = {0};
    char blank_string[MAX_LINE_LENGTH] = {0};

    int line_skip = 0;
    bool found_next_dir = false;

    if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "w+")))
    {
        fgets(next_line, MAX_LINE_LENGTH, file);
        fputs(next_line, buffer_file);
        line_skip++;

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

            if ((file = fopen(filename, "r")) && (buffer_file = fopen(BufferFileName, "a")))
            {
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

                        if (strlen(current_dir_name) == 0)
                        {

                            strncpy(current_dir_name, temp_line, MAX_LINE_LENGTH);
                        }

                        if (strncmp(current_dir_name, temp_line, strlen(current_dir_name)) == 0)
                        {
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
            if (!found)
            {
                int one_up_slash_index = index_of_one_up_slash(current_dir_name);
                char temp_dir_name[MAX_LINE_LENGTH] = {0};
                if (one_up_slash_index > 0)
                {
                    strncpy(temp_dir_name, current_dir_name, one_up_slash_index + 1);
                    strncpy(current_dir_name, temp_dir_name, MAX_LINE_LENGTH);
                }
                else
                {
                    strncpy(current_dir_name, blank_string, MAX_LINE_LENGTH);
                    line_skip++;
                    if (!found_next_dir)
                    {
                        searching = false;
                    }
                }
            }
        }

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
    sort_by_slashes(filename);
    sort_by_dirs_files(filename);
    printf("Filesystem %s has been sorted into file tree sequence.\n", filename);
    return EXIT_SUCCESS;
}
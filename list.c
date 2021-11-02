#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_ATTR_LENGTH 255

//counts how many subdirectories there are 1 level below
int count_links(char *name, char *filename)
{
    int links = 0;
    FILE *temp_file;
    char temp_line[MAX_ATTR_LENGTH] = {0};
    int index_of_slash = -1;    //index of the last slash
    int dir_slash_count = 0;    //counts how many slashes in the main dir
    int subdir_slash_count = 0; //counts how many slashes in the main dir

    char temp_path[MAX_ATTR_LENGTH] = {0};
    strncpy(temp_path, name, MAX_ATTR_LENGTH);

    //counts slash in main dir and get slash index
    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            index_of_slash = i;
            dir_slash_count++;
        }
    }

    //extracts dir 1 level above
    char dir_of_file[MAX_ATTR_LENGTH] = {0};
    strncpy(dir_of_file, temp_path, index_of_slash + 1);

    if ((temp_file = fopen(filename, "r")))
    {
        while (fgets(temp_line, MAX_ATTR_LENGTH, temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0';

            //counts slash for current dir
            for (int i = 0; i <= strlen(temp_line); i++)
            {
                if (temp_line[i] == '/')
                {
                    subdir_slash_count++;
                }
            }

            if ((strncmp(dir_of_file, temp_line, index_of_slash + 1) == 0)) //compare to see if the current dir contains the main dir in name
            {
                int slash_diff = subdir_slash_count - dir_slash_count; //this represents how many levels there are between directories
                if (slash_diff == 1)                                   //if it is one then counts toward the hard links
                {
                    links++;
                }
            }
            subdir_slash_count = 0; //resets this for every loop
        }

        fclose(temp_file);
    }

    return links;
}

//counts how many lines of content in a file
int count_lines(char *name, char *filename)
{
    int lines = 0;
    FILE *temp_file;
    char temp_line[MAX_ATTR_LENGTH] = {0};
    bool reading_a_file = false;

    if ((temp_file = fopen(filename, "r")))
    {
        while (fgets(temp_line, MAX_ATTR_LENGTH, temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0';

            // if file found
            if (strcmp(name, temp_line) == 0)
            {
                // then starts reading
                reading_a_file = true;
                while ((fgets(temp_line, MAX_ATTR_LENGTH, temp_file)) && reading_a_file)
                {
                    temp_line[strcspn(temp_line, "\n")] = '\0';
                    if (temp_line[0] == ' ') // if next line start with a space then counts line
                    {
                        lines++;
                    }
                    else // if not, stop reading
                    {
                        reading_a_file = false;
                    }
                }
            }
        }
        fclose(temp_file);
    }

    return lines;
}
int list(char *filename)
{
    char permissions[MAX_ATTR_LENGTH] = {0};
    char owners[MAX_ATTR_LENGTH] = {0};
    char groups[MAX_ATTR_LENGTH] = {0};
    char date[MAX_ATTR_LENGTH] = {0};

    struct stat fileStat; // attributes of FS file
    if (stat(filename, &fileStat) < 0)
    {
        fprintf(stderr, "Error! Can not get data attributes from the notes file. Please try again\n");
        exit(EX_NOINPUT);
    }

    // get file permissions like ls format
    (fileStat.st_mode & S_IRUSR) ? (permissions[1] = 'r') : (permissions[1] = '-');
    (fileStat.st_mode & S_IWUSR) ? (permissions[2] = 'w') : (permissions[2] = '-');
    (fileStat.st_mode & S_IXUSR) ? (permissions[3] = 'x') : (permissions[3] = '-');
    (fileStat.st_mode & S_IRGRP) ? (permissions[4] = 'r') : (permissions[4] = '-');
    (fileStat.st_mode & S_IWGRP) ? (permissions[5] = 'w') : (permissions[5] = '-');
    (fileStat.st_mode & S_IXGRP) ? (permissions[6] = 'x') : (permissions[6] = '-');
    (fileStat.st_mode & S_IROTH) ? (permissions[7] = 'r') : (permissions[7] = '-');
    (fileStat.st_mode & S_IWOTH) ? (permissions[8] = 'w') : (permissions[8] = '-');
    (fileStat.st_mode & S_IXOTH) ? (permissions[9] = 'x') : (permissions[9] = '-');

    //gets owners and group data
    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);

    //gets current time
    time_t now = time(NULL);
    struct tm tmfile, tmnow;

    //gets ownsers and group name
    strncpy(owners, pw->pw_name, MAX_ATTR_LENGTH);
    strncpy(groups, gr->gr_name, MAX_ATTR_LENGTH);
    //gets time when the file was last modified
    strncpy(date, ctime(&fileStat.st_mtime), MAX_ATTR_LENGTH);

    //convert the time into local timezone
    localtime_r(&fileStat.st_mtime, &tmfile);
    localtime_r(&now, &tmnow);

    //formats the time like ls command
    if (tmfile.tm_year == tmnow.tm_year) //if the year is the current year, displays the month,date, hours and minutes
    {
        strftime(date, MAX_ATTR_LENGTH, "%b %e %H:%M", &tmfile);
    }
    else //if the year is not current year, displays months, dates, and year
    {
        strftime(date, MAX_ATTR_LENGTH, "%b %e  %Y", &tmfile);
    }

    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        char next_line[MAX_ATTR_LENGTH] = {0};

        // goes through FS file line by line
        while (fgets(next_line, MAX_ATTR_LENGTH, file))
        {
            next_line[strcspn(next_line, "\n")] = '\0';
            if (next_line[0] == '@') //if next line is a file
            {
                char dir_name[MAX_ATTR_LENGTH] = {0};
                strncpy(dir_name, next_line, MAX_ATTR_LENGTH);
                dir_name[0] = '\0';

                permissions[0] = '-';                        // first char of permission is '-'
                char hard_links[] = "  1";                   //hard links is '  1' (with 3 chars as specified in the specs)
                int size = count_lines(next_line, filename); //gets number of lines

                //displays file attributes to stdout
                for (int i = 0; i < MAX_ATTR_LENGTH; i++)
                {
                    printf("%c", permissions[i]);
                }
                printf(" %s %s %s %d %s ", hard_links, owners, groups, size, date);

                for (int i = 0; i < MAX_ATTR_LENGTH; i++)
                {
                    printf("%c", dir_name[i]);
                }

                printf("\n");
            }
            else if (next_line[0] == '=') //if next line is a dir
            {
                char dir_name[MAX_ATTR_LENGTH] = {0};
                strncpy(dir_name, next_line, MAX_ATTR_LENGTH);
                dir_name[0] = '\0';

                permissions[0] = 'd';                              //first char of permisions is 'd'
                int hard_links = count_links(next_line, filename); //gets number of dir 1 level below
                int size = 0;                                      //size is 0 because this is a dir

                //displays file attributes to stdout
                for (int i = 0; i < MAX_ATTR_LENGTH; i++)
                {
                    printf("%c", permissions[i]);
                }
                printf(" %d %s %s %d %s ", hard_links, owners, groups, size, date);

                for (int i = 0; i < MAX_ATTR_LENGTH; i++)
                {
                    printf("%c", dir_name[i]);
                }

                printf("\n");
            }
        }
        fclose(file);
    }
    else
    {
        fprintf(stderr, "Error! Can not access the notes file. Please try again\n");
        exit(EX_NOINPUT);
    }

    return EXIT_SUCCESS;
}
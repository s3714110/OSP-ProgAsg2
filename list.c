#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_ATTR_LENGTH 255

int count_links(char *name, char *filename)
{
    int links = 0;
    FILE *temp_file;
    char temp_line[MAX_ATTR_LENGTH] = {0};
    int index_of_slash = -1;
    int dir_slash_count = 0;
    int subdir_slash_count = 0;

    char temp_path[MAX_ATTR_LENGTH] = {0};
    strncpy(temp_path, name, MAX_ATTR_LENGTH);

    for (int i = 0; i <= strlen(temp_path); i++)
    {
        if (temp_path[i] == '/')
        {
            index_of_slash = i;
            dir_slash_count++;
        }
    }

    
    char dir_of_file[MAX_ATTR_LENGTH] = {0};
    strncpy(dir_of_file, temp_path, index_of_slash + 1);

    if ((temp_file = fopen(filename, "r")))
    {
        while (fgets(temp_line, MAX_ATTR_LENGTH, temp_file))
        {
            temp_line[strcspn(temp_line, "\n")] = '\0';

            for (int i = 0; i <= strlen(temp_line); i++)
            {
                if (temp_line[i] == '/')
                {
                    subdir_slash_count++;
                }
            }

            if ((strncmp(dir_of_file, temp_line, index_of_slash + 1) == 0))
            {
                int slash_diff = subdir_slash_count - dir_slash_count;
                if (slash_diff == 1)
                {
                    links++;
                }
            }
            subdir_slash_count = 0;
        }

        fclose(temp_file);
    }

    return links;
}

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

            if (strcmp(name, temp_line) == 0)
            {
                reading_a_file = true;
                while ((fgets(temp_line, MAX_ATTR_LENGTH, temp_file)) && reading_a_file)
                {
                    temp_line[strcspn(temp_line, "\n")] = '\0';
                    if (temp_line[0] == ' ')
                    {
                        lines++;
                    }
                    else
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

    struct stat fileStat;
    if (stat(filename, &fileStat) < 0)
    {
        exit(EXIT_FAILURE);
    }

    (fileStat.st_mode & S_IRUSR) ? (permissions[1] = 'r') : (permissions[1] = '-');
    (fileStat.st_mode & S_IWUSR) ? (permissions[2] = 'w') : (permissions[2] = '-');
    (fileStat.st_mode & S_IXUSR) ? (permissions[3] = 'x') : (permissions[3] = '-');
    (fileStat.st_mode & S_IRGRP) ? (permissions[4] = 'r') : (permissions[4] = '-');
    (fileStat.st_mode & S_IWGRP) ? (permissions[5] = 'w') : (permissions[5] = '-');
    (fileStat.st_mode & S_IXGRP) ? (permissions[6] = 'x') : (permissions[6] = '-');
    (fileStat.st_mode & S_IROTH) ? (permissions[7] = 'r') : (permissions[7] = '-');
    (fileStat.st_mode & S_IWOTH) ? (permissions[8] = 'w') : (permissions[8] = '-');
    (fileStat.st_mode & S_IXOTH) ? (permissions[9] = 'x') : (permissions[9] = '-');

    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);

    time_t now = time(NULL);
    struct tm tmfile, tmnow;

    strncpy(owners, pw->pw_name, MAX_ATTR_LENGTH);
    strncpy(groups, gr->gr_name, MAX_ATTR_LENGTH);
    strncpy(date, ctime(&fileStat.st_mtime), MAX_ATTR_LENGTH);

    localtime_r(&fileStat.st_mtime, &tmfile);
    localtime_r(&now, &tmnow);

    if (tmfile.tm_year == tmnow.tm_year)
    {
        strftime(date, MAX_ATTR_LENGTH, "%b %e %H:%M", &tmfile);
    }
    else
    {
        strftime(date, MAX_ATTR_LENGTH, "%b %e  %Y", &tmfile);
    }

    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        char next_line[MAX_ATTR_LENGTH] = {0};

        while (fgets(next_line, MAX_ATTR_LENGTH, file))
        {
            next_line[strcspn(next_line, "\n")] = '\0';
            if (next_line[0] == '@')
            {
                char dir_name[MAX_ATTR_LENGTH] = {0};
                strncpy(dir_name, next_line, MAX_ATTR_LENGTH);
                dir_name[0] = '\0';

                permissions[0] = '-';
                int hard_links = 1;
                int size = count_lines(next_line, filename);

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
            else if (next_line[0] == '=')
            {
                char dir_name[MAX_ATTR_LENGTH] = {0};
                strncpy(dir_name, next_line, MAX_ATTR_LENGTH);
                dir_name[0] = '\0';

                permissions[0] = 'd';
                int hard_links = count_links(next_line, filename);
                int size = 0;

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

    return EXIT_SUCCESS;
}
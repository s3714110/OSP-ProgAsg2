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

    printf("Permission: ");
    for (int i = 0; i < MAX_ATTR_LENGTH; i++)
    {
        printf("%c", permissions[i]);
    }

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
    
    printf("Owner: %s\n", owners);
    printf("Group: %s\n", groups);
    printf("Date: %s\n", date);

    return EXIT_SUCCESS;
}
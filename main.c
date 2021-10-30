#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "init.h"
#include "list.h"
#include "mkdir.h"

#define MenuBreakTime 0
#define DefaultFilesystemName "FS.notes"
#define MAX_PATH_LENGTH 255

int fs_check(char *fs)
{
    int valid = 0;

    if ((fs[strlen(fs) - 1] != '/'))
    {
        if ((strcmp(fs, ".") != 0) && (strcmp(fs, "..") != 0) && (strcmp(fs, "/") != 0))
        {
            valid = 1;
        }
    }

    return valid;
}

void create_directory(char *path_name)
{
    char dir_name[MAX_PATH_LENGTH] = {0};
    int index_of_slash = -1;

    for (int i = 0; i <= strlen(path_name); i++)
    {
        if (path_name[i] == '/')
        {
            index_of_slash = i;
        }
    }

    if (index_of_slash != -1)
    {
        strncpy(dir_name, path_name, index_of_slash + 1);
        struct stat st = {0};

        if (stat(dir_name, &st) == -1)
        {
            printf("Path %s not found. Creating %s ...\n", dir_name, dir_name);
            mkdir(dir_name, 0777);
        }
    }
}

int main(int argc, char *argv[])
{

    fprintf(stdout, "VSFS program for OSP Assignment 2, made by Lam Tran, student id: s3714110\n");

    if (argc >= 3)
    {
        if (fs_check(argv[2]) == 0)
        {
            fprintf(stderr, "Error! Invalid path to notes file. Please try again\n");
            exit(EXIT_FAILURE);
        }

        create_directory(argv[2]);
        if (strcmp(argv[1], "list") == 0)
        {
            if (argc == 3)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running list function for filesystem %s ...\n", argv[2]);
                sleep(MenuBreakTime);
                list(argv[2]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[1], "copyin") == 0)
        {
            fprintf(stdout, "VSFS is now running copyin function...\n");
            sleep(MenuBreakTime);
        }
        else if (strcmp(argv[1], "copyout") == 0)
        {
            fprintf(stdout, "VSFS is now running copyout function...\n");
            sleep(MenuBreakTime);
        }
        else if (strcmp(argv[1], "mkdir") == 0)
        {
            if (argc == 4)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running mkdir to create directory %s for filesystem %s...\n", argv[3], argv[2]);
                sleep(MenuBreakTime);
                make_dir(argv[2], argv[3]);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[1], "rm") == 0)
        {
            if (argc == 4)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running rm function...\n");
                sleep(MenuBreakTime);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[1], "rmdir") == 0)
        {
            if (argc == 4)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running rmdir function...\n");
                sleep(MenuBreakTime);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[1], "defrag") == 0)
        {
            fprintf(stdout, "VSFS is now running defrag function...\n");
            sleep(MenuBreakTime);
        }
        else if (strcmp(argv[1], "index") == 0)
        {
            fprintf(stdout, "VSFS is now running index function...\n");
            sleep(MenuBreakTime);
        }

        else
        {
            fprintf(stderr, "Error! Invalid argument. Please refer to the documentations for list of arguments\n");
            exit(EXIT_FAILURE);
        }
    }

    else
    {
        fprintf(stderr, "Error! Program is missing arguments. Please refer to the documentations for list of arguments\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
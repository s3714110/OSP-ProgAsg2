#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sysexits.h>

#include "init.h"
#include "list.h"
#include "mkdir.h"
#include "rm.h"
#include "defrag.h"
#include "index.h"
#include "copy.h"

#define MenuBreakTime 0
#define DefaultFilesystemName "FS.notes"
#define MAX_PATH_LENGTH 255

int file_name_check(char *name)
{
    int valid = 0;

    if ((name[strlen(name) - 1] != '/'))
    {
        if ((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0) && (strcmp(name, "/") != 0))
        {
            valid = 1;
        }
    }

    return valid;
}

int dir_name_check(char *name)
{
    int valid = 0;

    if ((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0) && (strcmp(name, "/") != 0))
    {
        valid = 1;
    }

    return valid;
}

int make_recursive_path(char *path, mode_t mode)
{
    assert(path && *path);
    for (char *p = strchr(path + 1, '/'); p; p = strchr(p + 1, '/'))
    {
        *p = '\0';
        if (mkdir(path, mode) == -1)
        {
            if (errno != EEXIST)
            {
                *p = '/';
                return -1;
            }
            else
            {
                printf("Path %s already exists. No need to be automatically created\n", path);
            }
        }
        else
        {
            printf("Path %s has been automatically created.\n", path);
        }
        *p = '/';
    }
    return 0;
}

int create_directory(char *path_name)
{
    char dir_name[MAX_PATH_LENGTH] = {0};
    int index_of_slash = -1;
    int error_code = 0;

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
        printf("Checking path %s and creating any intermediate directories as needed...\n", dir_name);
        error_code = make_recursive_path(dir_name, 0777);
    }

    return error_code;
}

int main(int argc, char *argv[])
{

    fprintf(stdout, "VSFS program for OSP Assignment 2, made by Lam Tran, student id: s3714110\n");

    if (argc >= 3)
    {
        if (file_name_check(argv[2]) == 0)
        {
            fprintf(stderr, "Error! Invalid path to notes file. Please try again\n");
            exit(EX_DATAERR);
        }

        if (create_directory(argv[2]) != 0)
        {
            fprintf(stderr, "Error! Path to note file can not be accessed or created. Please try again\n");
            exit(EX_CANTCREAT);
        }

        if (strcmp(argv[1], "list") == 0)
        {
            if (argc == 3)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running list function for filesystem %s ...\n\n", argv[2]);
                sleep(MenuBreakTime);
                list(argv[2]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "copyin") == 0)
        {
            if (argc == 5)
            {
                if (file_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! Invalid path to external file. Please try again\n");
                    exit(EX_DATAERR);
                }

                if (file_name_check(argv[4]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with internal file name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running copyin to copy external file %s into internal file %s on filesystem %s...\n\n", argv[3],argv[4],argv[2]);
                sleep(MenuBreakTime);
                copy_in_b64(argv[2], argv[3], argv[4]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "copyin_plain") == 0)
        {
            if (argc == 5)
            {
                if (file_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! Invalid path to external file. Please try again\n");
                    exit(EX_DATAERR);
                }

                if (file_name_check(argv[4]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with internal file name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running copyin to copy external file %s into internal file %s on filesystem %s...\n\n", argv[3],argv[4],argv[2]);
                sleep(MenuBreakTime);
                copy_in(argv[2], argv[3], argv[4]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "copyout") == 0)
        {
            if (argc == 5)
            {
                if (file_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with internal file name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                if (file_name_check(argv[4]) == 0)
                {
                    fprintf(stderr, "Error! Invalid path to external file. Please try again\n");
                    exit(EX_DATAERR);
                }

                if (create_directory(argv[4]) != 0)
                {
                    fprintf(stderr, "Error! Path to external file can not be accessed or created. Please try again\n");
                    exit(EX_CANTCREAT);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running copyout to copy internal file %s on filesytem %s out to external file %s...\n\n",argv[3],argv[2],argv[4]);
                sleep(MenuBreakTime);
                copy_out(argv[2], argv[3], argv[4]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "copyout_plain") == 0)
        {
            if (argc == 5)
            {
                if (file_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with internal file name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                if (file_name_check(argv[4]) == 0)
                {
                    fprintf(stderr, "Error! Invalid path to external file. Please try again\n");
                    exit(EX_DATAERR);
                }

                if (create_directory(argv[4]) != 0)
                {
                    fprintf(stderr, "Error! Path to external file can not be accessed or created. Please try again\n");
                    exit(EX_CANTCREAT);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running copyout to copy internal file %s on filesytem %s out to external file %s...\n\n",argv[3],argv[2],argv[4]);
                sleep(MenuBreakTime);
                copy_out(argv[2], argv[3], argv[4]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "mkdir") == 0)
        {
            if (argc == 4)
            {
                if (dir_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with given directory name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running mkdir to create directory %s for filesystem %s...\n\n", argv[3], argv[2]);
                sleep(MenuBreakTime);
                make_dir(argv[2], argv[3]);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "rm") == 0)
        {
            if (argc == 4)
            {
                if (file_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with given file name. Please try again!\n");
                    exit(EX_DATAERR);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running rm to remove file %s for filesystem %s ...\n\n", argv[3], argv[2]);
                sleep(MenuBreakTime);
                remove_file(argv[2], argv[3]);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "rmdir") == 0)
        {
            if (argc == 4)
            {
                if (dir_name_check(argv[3]) == 0)
                {
                    fprintf(stderr, "Error! There is a syntax error with given directory name. Please try again!\n\n");
                    exit(EX_DATAERR);
                }

                init(argv[2]);
                fprintf(stdout, "VSFS is now running rmdir to remove directory %s for filesystem %s ...\n", argv[3], argv[2]);
                sleep(MenuBreakTime);
                remove_dir(argv[2], argv[3]);
            }

            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "defrag") == 0)
        {

            if (argc == 3)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running defrag function for filesystem %s ...\n\n", argv[2]);
                sleep(MenuBreakTime);
                defrag(argv[2]);
                sort(argv[2]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        else if (strcmp(argv[1], "index") == 0)
        {
            if (argc == 3)
            {
                init(argv[2]);
                fprintf(stdout, "VSFS is now running index function for filesystem %s ...\n\n", argv[2]);
                sleep(MenuBreakTime);
                create_index(argv[2]);
                read_index(argv[2]);
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }

        else
        {
            fprintf(stderr, "Error! Invalid argument. Please refer to the documentations for list of arguments\n");
            exit(EX_USAGE);
        }
    }

    else
    {
        fprintf(stderr, "Error! Program is missing arguments. Please refer to the documentations for list of arguments\n");
        exit(EX_USAGE);
    }

    return EXIT_SUCCESS;
}
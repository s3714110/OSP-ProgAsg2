#define _GNU_SOURCE
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

int is_file_gz(char *filename)
{
    int is_gz = 0;
    char file_extensions[3] = {0};

    if (strlen(filename) > 3)
    {
        strncpy(file_extensions, filename + (strlen(filename) - 3), 3);
        if (strcmp(file_extensions, ".gz") == 0)
        {
            is_gz = 1;
            printf("%s is a gz file\n", filename);
        }
    }

    return is_gz;
}

int zip_file(char *filename)
{
    int failure = 1;
    char cmd[MAX_PATH_LENGTH] = {0};
    strcat(cmd, "gzip -f ");
    strcat(cmd, filename);

    FILE *output;

    if ((output = popen(cmd, "r")))
    {
        printf("Compressing %s ...\n", filename);
    }
    else
    {
        failure = 1;
        fprintf(stderr, "Cannot run %s ! Please try again \n", cmd);
        exit(EX_OSERR);
    }

    if (pclose(output) == 0)
    {
        printf("File %s has been succesfully compressed into a .gz file\n", filename);
        failure = 0;
    }
    else
    {
        failure = 1;
        fprintf(stderr, "Error! Program is now exitting...\n");
        exit(EX_SOFTWARE);
    }

    return failure;
}

int upzip_file(char *filename)
{
    int failure = 1;
    char cmd[MAX_PATH_LENGTH] = {0};
    strcat(cmd, "gunzip -f ");
    strcat(cmd, filename);

    FILE *output;

    if ((output = popen(cmd, "r")))
    {
        printf("Uncompressing %s ...\n", filename);
    }
    else
    {
        failure = 1;
        fprintf(stderr, "Cannot run %s ! Please try again \n", cmd);
        exit(EX_OSERR);
    }

    if (pclose(output) == 0)
    {
        printf("File %s has been succesfully uncompressed\n", filename);
        failure = 0;
    }
    else
    {
        failure = 1;
        fprintf(stderr, "Error! Program is now exitting...\n");
        exit(EX_SOFTWARE);
    }

    return failure;
}

int main(int argc, char *argv[])
{

    fprintf(stdout, "VSFS program for OSP Assignment 2, made by Lam Tran, student id: s3714110\n");

    if (argc >= 3)
    {
        char filesystem_name[MAX_PATH_LENGTH] = {0};

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

        if (is_file_gz(argv[2]) == 0)
        {
            strncpy(filesystem_name, argv[2], strlen(argv[2]));
        }
        else
        {
            if (upzip_file(argv[2]) == 0)
            {
                strncpy(filesystem_name, argv[2], strlen(argv[2]) - 3);
                printf("Program is now using uncompressed filesystem %s ...\n", filesystem_name);
            }
            else
            {
                fprintf(stderr, "Error! Program is now exitting...\n");
                exit(EX_SOFTWARE);
            }
        }

        if (strcmp(argv[1], "list") == 0)
        {
            if (argc == 3)
            {
                init(filesystem_name);
                fprintf(stdout, "VSFS is now running list function for filesystem %s ...\n\n", filesystem_name);
                sleep(MenuBreakTime);
                list(filesystem_name);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running copyin to copy external file %s into internal file %s on filesystem %s...\n\n", argv[3], argv[4], filesystem_name);
                sleep(MenuBreakTime);
                copy_in_b64(filesystem_name, argv[3], argv[4]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running copyin to copy external file %s into internal file %s on filesystem %s...\n\n", argv[3], argv[4], filesystem_name);
                sleep(MenuBreakTime);
                copy_in(filesystem_name, argv[3], argv[4]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running copyout to copy internal file %s on filesytem %s out to external file %s...\n\n", argv[3], filesystem_name, argv[4]);
                sleep(MenuBreakTime);
                copy_out_b64(filesystem_name, argv[3], argv[4]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running copyout to copy internal file %s on filesytem %s out to external file %s...\n\n", argv[3], filesystem_name, argv[4]);
                sleep(MenuBreakTime);
                copy_out(filesystem_name, argv[3], argv[4]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running mkdir to create directory %s for filesystem %s...\n\n", argv[3], filesystem_name);
                sleep(MenuBreakTime);
                make_dir(filesystem_name, argv[3]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running rm to remove file %s for filesystem %s ...\n\n", argv[3], filesystem_name);
                sleep(MenuBreakTime);
                remove_file(filesystem_name, argv[3]);
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

                init(filesystem_name);
                fprintf(stdout, "VSFS is now running rmdir to remove directory %s for filesystem %s ...\n", argv[3], filesystem_name);
                sleep(MenuBreakTime);
                remove_dir(filesystem_name, argv[3]);
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
                init(filesystem_name);
                fprintf(stdout, "VSFS is now running defrag function for filesystem %s ...\n\n", filesystem_name);
                sleep(MenuBreakTime);
                defrag(filesystem_name);
                sort(filesystem_name);
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
                init(filesystem_name);
                fprintf(stdout, "VSFS is now running index function for filesystem %s ...\n\n", filesystem_name);
                sleep(MenuBreakTime);
                create_index(filesystem_name);
                read_index(filesystem_name);
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
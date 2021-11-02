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
#define MAX_PATH_LENGTH 255

// checks for valid format of the filesystem name
int file_name_check(char *name)
{
    int valid = 0;

    if ((name[strlen(name) - 1] != '/'))    // can't end with '/'
    {
        if ((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0) && (strcmp(name, "/") != 0))  // can't be exactly any 1 of these chars
        {
            valid = 1;
        }
    }

    return valid;
}

// checks for valid format of directory name
int dir_name_check(char *name)
{
    int valid = 0;

    if ((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0) && (strcmp(name, "/") != 0))  // can't be exactly any 1 of these chars
    {
        valid = 1;
    }

    return valid;
}

// creates recursive directories if the given directory path doesnt exist
int make_recursive_path(char *path, mode_t mode)
{
    assert(path && *path);
    for (char *p = strchr(path + 1, '/'); p; p = strchr(p + 1, '/'))    // breaks path into sections with '/' as delimitter
    {
        *p = '\0';
        if (mkdir(path, mode) == -1)    // creates 1 directory at a time
        {
            if (errno != EEXIST)    // if errno is not eexist, then it means mkdir gives an error and can't create directory  
            {
                *p = '/';
                return -1;
            }
            else    // if directory already exists, do nothing
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

//creates directory given filepath
int create_directory(char *path_name)
{
    char dir_name[MAX_PATH_LENGTH] = {0};
    int index_of_slash = -1;
    int error_code = 0;

    // checks to see if the path is at root folder of fs
    for (int i = 0; i <= strlen(path_name); i++)
    {
        if (path_name[i] == '/')
        {
            index_of_slash = i;
        }
    }

    // if not then get directory that contains the fails, and calls mkdir recursively as needed
    if (index_of_slash != -1)
    {
        strncpy(dir_name, path_name, index_of_slash + 1);
        printf("Checking path %s and creating any intermediate directories as needed...\n", dir_name);
        error_code = make_recursive_path(dir_name, 0777);
    }

    return error_code;
}

// checks to see if filename ends with .gz
int is_file_gz(char *filename)
{
    int is_gz = 0;
    char file_extensions[MAX_PATH_LENGTH] = {0};

    if (strlen(filename) > 3)
    {
        strncpy(file_extensions, filename + (strlen(filename) - 3), MAX_PATH_LENGTH);
        if (strcmp(file_extensions, ".gz") == 0)
        {   
            
            is_gz = 1;
        }
    }

    return is_gz;
}

// compresses a file using gzip
int zip_file(char *filename)
{
    int failure = 1;

    // setting command line
    char cmd[MAX_PATH_LENGTH] = {0};
    strcat(cmd, "gzip -f ");
    strcat(cmd, filename);

    FILE *output;

    if ((output = popen(cmd, "r")))
    {
        printf("Compressing %s ...\n", filename);
    }
    else    // indicate popens can not run the command
    {
        failure = 1;
        fprintf(stderr, "Cannot run %s ! Please try again \n", cmd);
        exit(EX_OSERR);
    }

    if (pclose(output) == 0)    // if the exit code of popen is 0 then gzip has succeeded
    {
        printf("File %s has been succesfully compressed into a .gz file\n", filename);
        failure = 0;
    }
    else    // if exit code is not 0 then gzip command has failed, maybe due to invalid format, file doesnt exist, etc...
    {
        failure = 1;
        fprintf(stderr, "Error! Program is now exitting...\n");
        exit(EX_SOFTWARE);
    }

    return failure;
}

// Uncompress a gz file using gunzip. Similar to zip method above
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

    if (argc >= 3)  // all possible command requires at least 3 arguments, name of program, FS name, and option(s)
    {
        char filesystem_name[MAX_PATH_LENGTH] = {0};

        if (file_name_check(argv[2]) == 0)  // checks filesystem name
        {
            fprintf(stderr, "Error! Invalid path to notes file. Please try again\n");
            exit(EX_DATAERR);
        }

        if (create_directory(argv[2]) != 0) // creates directory fs if needed
        {
            fprintf(stderr, "Error! Path to note file can not be accessed or created. Please try again\n");
            exit(EX_CANTCREAT);
        }

        if (is_file_gz(argv[2]) == 0)   // checks to see if a file is a .gz file
        {
            strncpy(filesystem_name, argv[2], strlen(argv[2])); // if not then fs name is the same as provided
        }
        else
        {   

            printf("%s is a gz file and will need to be uncompressed\n", argv[2]);
            if (upzip_file(argv[2]) == 0)   //unzip fs file
            {
                strncpy(filesystem_name, argv[2], strlen(argv[2]) - 3); // now using uncompressed fs without .gz in the name
                printf("Program is now using uncompressed filesystem %s ...\n", filesystem_name);
            }
            else
            {
                fprintf(stderr, "Error! Program is now exitting...\n");
                exit(EX_SOFTWARE);
            }
        }

        // options for vsfs commands
        // for every option, if the arguments provided are correct, then runs init, which checks the format of FS file before running its own function

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
        //by default copyin will use base64 encode
        else if (strcmp(argv[1], "copyin") == 0)
        {
            if (argc == 5)
            {
                if (file_name_check(argv[3]) == 0)  // checks filename of external file
                {
                    fprintf(stderr, "Error! Invalid path to external file. Please try again\n");
                    exit(EX_DATAERR);
                }

                if (file_name_check(argv[4]) == 0)  // checks filename of internal file
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
        //use this option for plain copy of file
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
        // same as copyin, use base64 decode by default
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

                if (create_directory(argv[4]) != 0) // checks to see if directory of external file needs to be created before being copied out
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
        // plain file copy for copyout
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
                if (dir_name_check(argv[3]) == 0)   // checks dir name 
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
                if (file_name_check(argv[3]) == 0)  // checks file name 
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
                if (dir_name_check(argv[3]) == 0)   //check dir name
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
                defrag(filesystem_name);    // removes deleted entries first
                sort(filesystem_name);  // then sorts into file tree sequence
            }
            else
            {
                fprintf(stderr, "Error! Invalid format. Please refer to the documentations for list of arguments\n");
                exit(EX_USAGE);
            }
        }
        // this option is not needed, because this vsfs implementation doesn't need index
        // so this option will simply create an index file that indicates which line in the filesystem does a file or directory start, and list them out
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

        // if the provided file name is a gz file, then compress the file again before program exits
        if (is_file_gz(argv[2]) == 1)
        {
            printf("\nUncompressed file %s will need to be compressed back into a .gz file\n",filesystem_name);
            if (zip_file(filesystem_name) == 0)
            {
                printf("Filesystem has been compressed into %s \n", argv[2]);
            }
            else
            {
                fprintf(stderr, "Error! Program is now exitting...\n");
                exit(EX_SOFTWARE);
            }
        }
    }

    else
    {
        fprintf(stderr, "Error! Program is missing arguments. Please refer to the documentations for list of arguments\n");
        exit(EX_USAGE);
    }

    return EXIT_SUCCESS;
}
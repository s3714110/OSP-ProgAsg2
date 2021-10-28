#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "init.h"

#define MenuBreakTime 1
#define DefaultFilesystemName "FS.notes"


int main(int argc, char *argv[])
{

    fprintf(stdout, "VSFS program for OSP Assignment 2, made by Lam Tran, student id: s3714110\n");
    init(DefaultFilesystemName);

    if (argc >= 2)
    {
        if (strcmp(argv[1], "list") == 0)
        {
            fprintf(stdout, "VSFS is now running list function...\n");
            sleep(MenuBreakTime);
          
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
            fprintf(stdout, "VSFS is now running mkdir function...\n");
            sleep(MenuBreakTime);
            
        }
        else if (strcmp(argv[1], "rm") == 0)
        {
            fprintf(stdout, "VSFS is now running rm function...\n");
            sleep(MenuBreakTime);
            
        }
        else if (strcmp(argv[1], "rmdir") == 0)
        {
            fprintf(stdout, "VSFS is now running rmdir function...\n");
            sleep(MenuBreakTime);
            
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
            fprintf(stderr, "Invalid argument. Please refer to the documentations for list of arguments\n");
            return EXIT_FAILURE;
        }
    }

    else
    {
        fprintf(stderr, "Program needs an argument. Please refer to the documentations for list of arguments\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
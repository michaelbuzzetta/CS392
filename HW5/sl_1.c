/*******************************************************************************
 * Name : sl.c
 * Author : Michael Buzzetta
 * Date : April 4, 2024
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
/*
CAs that helped:
Ryan Monaghan
Eddison So
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (stat(argv[1], &st) == -1)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "Permission denied. Cannot read the directory.");
            exit(EXIT_FAILURE);
        }
    }
    if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "The first argument has to be a directory.");
        exit(EXIT_FAILURE);
    }
    if (!(st.st_mode & S_IRUSR))
    {
        fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
        exit(EXIT_FAILURE);
    }

    int total_files = 0;

    //reused code from minishell, chenged for my needs
    DIR *dir;
    struct dirent *lf;
    if ((dir = opendir(argv[1])) != NULL)
    {
        while ((lf = readdir(dir)) != NULL)
        {
            total_files++;
        }
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
        exit(EXIT_FAILURE);
    }
    //end reused code

    int ls_sort[2];
    if (pipe(ls_sort) == -1)
    {
        fprintf(stderr, "Failed to create pipes.\n");
        exit(EXIT_FAILURE);
    }

    pid_t ls_pid = fork();
    if (ls_pid == -1)
    {
        fprintf(stderr, "Failed to fork.\n");
        exit(EXIT_FAILURE);
    }

    if (ls_pid == 0)
    {
        dup2(ls_sort[1], STDOUT_FILENO);
        close(ls_sort[0]);
        close(ls_sort[1]);
        execlp("ls", "ls", "-1ai", argv[1], NULL);
        fprintf(stderr, "ls failed.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(ls_sort[1]);
        pid_t sort_pid = fork();
        if (sort_pid == -1)
        {
            fprintf(stderr, "Failed to fork.\n");
            exit(EXIT_FAILURE);
        }

        if (sort_pid == 0)
        {
            dup2(ls_sort[0], STDIN_FILENO);
            close(ls_sort[0]);
            execlp("sort", "sort", NULL);
            fprintf(stderr, "sort failed.\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            close(ls_sort[0]);

            wait(NULL);
            wait(NULL);
            printf("Total files: %d\n", total_files);
        }
    }

    return 0;
}

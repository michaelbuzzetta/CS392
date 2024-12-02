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
Daniel Zamloot
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat st;
    DIR* direct = opendir(argv[1]);
    if(direct==NULL)
    {
        if(errno == EACCES || errno == ENOENT)
        {
            fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    if (stat(argv[1], &st) == -1)
    {
        fprintf(stderr, "Permission denied. %s cannot be read.", argv[1]);
        exit(EXIT_FAILURE);
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

    int lsSort[2];
    int sortPar[2];
    if (pipe(lsSort) == -1 || pipe(sortPar) == -1)
    {
        fprintf(stderr, "Error: pipe failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pid_t lsPid = fork();
    if (lsPid == -1)
    {
        fprintf(stderr, "Error: fork failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (lsPid == 0)
    {
        close(lsSort[0]);
        dup2(lsSort[1], STDOUT_FILENO);
        close(lsSort[1]);
        execlp("ls", "ls", "-1ai", argv[1], NULL);
        fprintf(stderr, "Error: execlp ls failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        close(lsSort[1]);
        pid_t sortPid = fork();
        if (sortPid == -1)
        {
            fprintf(stderr, "Error: fork failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (sortPid == 0)
        {
            close(lsSort[1]);
            dup2(lsSort[0], STDIN_FILENO);
            close(lsSort[0]);
            dup2(sortPar[1], STDOUT_FILENO);
            close(sortPar[0]);
            close(sortPar[1]);
            execlp("sort", "sort", NULL);
            fprintf(stderr, "Error: execlp sort failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        {
            close(sortPar[1]);
            dup2(sortPar[0], STDIN_FILENO);
            close(sortPar[0]);
            char temp[4096];
            ssize_t char_count;
            while ((char_count = read(STDIN_FILENO, temp, sizeof(temp))) > 0)
            {
                for (int i = 0; i < char_count; i++)
                {
                    putchar(temp[i]);
                    if (temp[i] == '\n')
                    {
                        total_files++;
                    }
                }
            }
            if (char_count == -1)
            {
                fprintf(stderr, "Error: read failed: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            printf("Total files: %d\n", total_files);
        }
    }

    return 0;
}

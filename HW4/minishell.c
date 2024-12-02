/*******************************************************************************
 * Name : minishell.c
 * Author : Michael Buzzetta
 * Date : March 20 2024
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

/*
CAs that helped:
Alex D'Agostino
Eddison So
Daniel Zamloot
Dean Zazzera
Rudolph Sedlin
Alex D'Agostino
Joshua Bernstein
Jesse Knuckles
*/

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

// taken from Canvas
#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"
volatile sig_atomic_t interupt = 0;
typedef struct proc_info
{
    int PID;
    char USER[1024];
    char COMMAND[1024];
} proc_info;

void signal_catch(int input)
{
    interupt = 1;
    printf("\n");
}

// void error_print(char* output, char* error)
// {
//     fprintf(stderr, "Error: %s%s\n", output, error);
// }

void working_directory()
{
    char command[2048];
    if (getcwd(command, sizeof(command)) != NULL)
    {
        printf("%s[%s%s%s]>%s", BLUE, command, DEFAULT, BLUE, DEFAULT);
    }
    else
    {
        fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
    }
}

void change_directory(char *path)
{
    if (path == NULL || strcmp(path, "~") == 0)
    {
        struct passwd *pwd = getpwuid(getuid());
        if (pwd == NULL)
        {
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
            return;
        }
        path = pwd->pw_dir;
    }

    if (chdir(path) != 0)
    {
        fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", path, strerror(errno));
    }
}

void files()
{
    DIR *dir;
    struct dirent *lf;
    if ((dir = opendir(".")) != NULL)
    {
        while ((lf = readdir(dir)) != NULL)
        {
            if (strcmp(lf->d_name, ".") != 0 && strcmp(lf->d_name, "..") != 0)
            {
                printf("%s\n", lf->d_name);
            }
        }
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Error: Failed to open directory. %s.\n", strerror(errno));
    }
}

// https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
// referenced and changed to fit my needs
int compare_proc(const void *a, const void *b)
{
    const proc_info *procA = (const proc_info *)a;
    const proc_info *procB = (const proc_info *)b;
    return (procA->PID - procB->PID);
}

void processes()
{
    DIR *proc;
    struct dirent *lp;
    struct stat temp;
    proc_info processes_array[2048];
    int num_processes = 0;

    if ((proc = opendir("/proc")) != NULL)
    {
        while ((lp = readdir(proc)) != NULL)
        {
            if (atoi(lp->d_name) != 0)
            {
                char proc_path[1024];
                char proc_path_1[2048];
                // printf("/proc/%s/cmdline\n", lp->d_name);
                snprintf(proc_path, sizeof(proc_path), "/proc/%s/cmdline", lp->d_name);
                snprintf(proc_path_1, sizeof(proc_path_1), "%s%s", "/proc/", lp->d_name);
                stat(proc_path_1, &temp);
                struct passwd *password = getpwuid(temp.st_uid);
                // printf("%s%s", "/proc/\n", lp->d_name);
                //printf("%s ", lp->d_name);
                //printf("%s ", password->pw_name);
                FILE *commandLine = fopen(proc_path, "r");
                if (commandLine != NULL)
                {
                    if (fscanf(commandLine, "%s", processes_array[num_processes].COMMAND) == 1)
                    {
                        processes_array[num_processes].PID = atoi(lp->d_name);
                        strcpy(processes_array[num_processes].USER, password->pw_name);
                        num_processes++;
                    }


                    // char filePath[2048];
                    // char *fileName;
                    // char recent[1024];
                    // fgets(filePath, sizeof(filePath), commandLine);
                    // // char *newLine = strchr(filePath, '\n');
                    // // if (newLine != NULL)
                    // // {
                    // //     newLine = '\0';
                    // // }
                    // // fileName = strtok(filePath, "/");
                    // // char *lastSlash = NULL;
                    // // while (fileName != NULL)
                    // // {
                    // //     strcpy(recent, fileName);
                    // //     fileName = strtok(NULL, "/");
                    // // }
                    // fileName=strtok(filePath, "/");
                    // char* lastSlash=NULL;
                    // while(fileName !=NULL);
                    // {
                    //     lastSlash=fileName;
                    //     fileName =strtok(NULL, "/");
                    // }
                    // if(lastSlash!=NULL)
                    // {
                    //     //printf("%s\n", filePath);
                    // }
                    // processes_array[num_processes].PID = atoi(lp->d_name);
                    // strcpy(processes_array[num_processes].USER, password->pw_name);
                    // strcpy(processes_array[num_processes].COMMAND, recent);
                    // num_processes++;
                    // char *fileName1 = strtok(filePath, "/");
                    // char *recent1 = NULL;
                    // while (fileName1 != NULL)
                    // {
                    //     recent1 = fileName1;
                    //     fileName1 = strtok(NULL, "/");
                    // }
                    // if (fileName1 != NULL)
                    // {
                    //     printf("%s\n", recent1);
                    // }

                    // printf("%s\n", recent);
                    // printf("%s\n", recent1);
                    //printf("%s\n", fileName);
                    fclose(commandLine);
                }
            }
        }
        closedir(proc);
        qsort(processes_array, num_processes, sizeof(proc_info), compare_proc);

        for (int i = 0; i < num_processes; i++)
        {
            printf("%d %s %s\n", processes_array[i].PID, processes_array[i].USER, processes_array[i].COMMAND);
        }
    }
    else
    {
        fprintf(stderr, "Error: Failed to open /proc directory. %s.\n", strerror(errno));
    }
}

int main()
{
    struct sigaction signal;
    signal.sa_handler = signal_catch;
    signal.sa_flags = 0;
    if (sigaction(SIGINT, &signal, NULL) == -1)
    {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    while (1)
    {
        char input[2048];

        working_directory();

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            if (interupt)
            {
                interupt = 0;
                printf("\n");
                continue;
            }
            else
            {
                fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }

        input[strcspn(input, "\n")] = '\0';
        char command[1024];
        char *args[1024];
        memset(command, 0, sizeof(command));
        memset(args, 0, sizeof(args));
        char *temp;
        // if(strlen(input)==0)
        // {
        //     continue;
        // }
        temp = strtok(input, " ");
        if (temp == NULL)
        {
            continue;
        }
        strncpy(command, temp, sizeof(command) - 1);
        args[0] = command;
        int x = 1;
        while ((temp = strtok(NULL, " ")) != NULL && x < sizeof(args) / sizeof(args[0]))
        {
            args[x] = temp;
            x++;
        }
        // int length = strlen(command);
        // printf("%d\n", length);

        if (command == NULL)
        {
            continue;
        }

        else if (strcmp(command, "exit") == 0)
        {
            return EXIT_SUCCESS;
        }
        else if (strcmp(command, "cd") == 0)
        {
            if (args[1] == NULL)
            {
                change_directory("~");
                // printf("i did this\n");
            }
            else if (strtok(NULL, "\n") != NULL)
            {
                fprintf(stderr, "Error: Too many arguments to cd. %s.\n", strerror(errno));
            }
            else
            {
                change_directory(args[1]);
            }
        }
        else if (strcmp(command, "pwd") == 0)
        {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            }
        }
        else if (strcmp(command, "lf") == 0)
        {
            files();
        }
        else if (strcmp(command, "lp") == 0)
        {
            processes();
        }
        else
        {
            pid_t ID = fork();
            if (ID == -1)
            {
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            else if (ID == 0)
            {
                execvp(command, args);
                // for (int i = 0; i < 5; i++)
                // {
                //     printf("%s\n", args[i]);
                // }
                // printf("Command: %s, Args[0]: %s\n", command, args[2]);
                fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            else
            {
                int status;
                if (waitpid(ID, &status, 0) == -1 && !interupt)
                {
                    fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Name : pfind.c
 * Author : Michael Buzzetta
 * Date : Feb 26 2024
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

/*
Recieved help from CAs during office hours:
Eddison So
Daniel Zamloot
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int input_check(char *input)
{
    if (strlen(input) != 9)
    {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", input);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 9; i++)
    {
        if ((i == 0 || i == 3 || i == 6) && input[i] != 'r' && input[i] != '-')
        {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", input);
            exit(EXIT_FAILURE);
        }
        else if ((i == 1 || i == 4 || i == 7) && input[i] != 'w' && input[i] != '-')
        {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", input);
            exit(EXIT_FAILURE);
        }
        else if ((i == 2 || i == 5 || i == 8) && input[i] != 'x' && input[i] != '-')
        {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", input);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void file_find(char *name, char *permission)
{
    DIR *directory = opendir(name);
    struct dirent *temp;
    struct stat file_permission;
    char track[2048];


    if (directory == NULL)
    {
        fprintf(stderr, "Error opening directory %s\n", name);
        exit(EXIT_FAILURE);
    }

    while ((temp = readdir(directory)) != NULL)
    {
        // referenced online resources for how to ignore . and ..
        if (strcmp(temp->d_name, ".") == 0 || strcmp(temp->d_name, "..") == 0)
        {
            continue;
        }

        strcpy(track, name);
        if(track[strlen(track)-1] !='/')
        {
            strcat(track, "/");
        }
        strcat(track, temp->d_name);
        int check = stat(track, &file_permission);

        if (S_ISDIR(file_permission.st_mode))
        {
            file_find(track, permission);
        }
        else
        {
            char temp[10];
            // referenced online resources on proper syntax for these checks
            temp[0] = (file_permission.st_mode & S_IRUSR) ? 'r' : '-';
            temp[1] = (file_permission.st_mode & S_IWUSR) ? 'w' : '-';
            temp[2] = (file_permission.st_mode & S_IXUSR) ? 'x' : '-';
            temp[3] = (file_permission.st_mode & S_IRGRP) ? 'r' : '-';
            temp[4] = (file_permission.st_mode & S_IWGRP) ? 'w' : '-';
            temp[5] = (file_permission.st_mode & S_IXGRP) ? 'x' : '-';
            temp[6] = (file_permission.st_mode & S_IROTH) ? 'r' : '-';
            temp[7] = (file_permission.st_mode & S_IWOTH) ? 'w' : '-';
            temp[8] = (file_permission.st_mode & S_IXOTH) ? 'x' : '-';
            temp[9] = '\0';

            if (strcmp(temp, permission) == 0)
            {
                printf("%s\n", track);
            }
        }
    }
    closedir(directory);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error: Invalid number of arguments.\nUsage: %s <directory> <permissions>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *path = argv[1];
    char *input = argv[2];
    //printf("Here\n");
    input_check(input);
    //printf("here1\n");
    file_find(path, input);

    return 0;
}
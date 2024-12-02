/*******************************************************************************
 * Name : client.c
 * Author : Michael Buzzetta
 * Date : April 18, 2024
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

/*
CAs that helped:
Ryan Monaghan
Eddison So
Daniel Zamloot
Dean Zazzera
Rudolph Sedlin
Alex D'Agostino
Jason Qiu
Marcos Traverso
Rudolph Sedlin
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 25555

void print_usage(char *prog_name)
{
    printf("Usage: %s [-i IP_address] [-p port_number] [-h]\n", prog_name);
    printf("-i IP_address      Default to \"127.0.0.1\"\n");
    printf("-p port_number     Default to 25555\n");
    printf("-h                 Display this help info\n");
}

void parse_connect(int argc, char** argv, int* server_fd)
{
    int opt;
    char* ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT;

    while ((opt = getopt(argc, argv, "i:p:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            ip_address = optarg;
            break;
        case 'p':
            port_number = atoi(optarg);
            break;
        case 'h':
            print_usage(argv[0]);
            exit(0);
        case '?':
            fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
            exit(1);
        }
    }

    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }
    else
    {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip_address);
        server_addr.sin_port = htons(port_number);

        if (connect(*server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("Connection failed");
            exit(1);
        }
    }
}


void handle_server_message(int server_fd)
{
    char buffer[1024];
    int bytes_received = recv(server_fd, buffer, sizeof(buffer), 0);
    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    else if (bytes_received == 0)
    {
        printf("Server disconnected.\n");
        exit(0);
    }
    else
    {
        perror("Error receiving message from server");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int server_fd;
    struct sockaddr_in server_addr;
    fd_set readfds;
    int max_fd;
    int activity;

    parse_connect(argc, argv, &server_fd);

    printf("Connected to server.\n");

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(server_fd, &readfds);

        max_fd = (STDIN_FILENO > server_fd) ? STDIN_FILENO : server_fd;

        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("Error in select");
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char input[1024];
            fgets(input, sizeof(input), stdin);
            send(server_fd, input, strlen(input), 0);
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            handle_server_message(server_fd);
        }
    }

    close(server_fd);

    return 0;
}

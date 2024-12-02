/*******************************************************************************
 * Name : server.c
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
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define DEFAULT_QUESTION_FILE "questions.txt"
#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 25555
#define MAX_CONNECTIONS 3

// copied form Canvas
struct Entry
{
    char prompt[1024];
    char options[3][50];
    int answer_idx;
};

// copied form Canvas
struct Player
{
    int fd;
    int score;
    char name[128];
};

void print_usage(char *prog_name)
{
    printf("Usage: %s [-f question_file] [-i IP_address] [-p port_number] [-h]\n", prog_name);
    printf("-f question_file   Default to \"question.txt\"\n");
    printf("-i IP_address      Default to \"127.0.0.1\"\n");
    printf("-p port_number     Default to 25555\n");
    printf("-h                 Display this help info\n");
}

int read_questions(struct Entry *arr, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    int track = 0;
    char buffer[1024];
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file))
    {
        line_number++;

        buffer[strcspn(buffer, "\n")] = '\0';

        int mod = (line_number - 1) % 4;
        switch (mod)
        {
        case 0:
            strcpy(arr[track].prompt, buffer);
            break;
        case 1:
        {
            char *token = strtok(buffer, " ");
            for (int i = 0; i < 3 && token != NULL; i++)
            {
                strcpy(arr[track].options[i], token);
                token = strtok(NULL, " ");
            }
            break;
        }
        case 2:
            for (int i = 0; i < 3; i++)
            {
                if (strcmp(arr[track].options[i], buffer) == 0)
                {
                    arr[track].answer_idx = i + 1;
                    break;
                }
            }
            track++;
            break;
        }
    }

    fclose(file);
    return track;
}

void server_print_question(int player_fd, struct Entry question, int question_number)
{
    char buffer[2048];
    printf("Question %d: %s\n\n", question_number, question.prompt);
    printf("Correct Answer: %d\n", question.answer_idx);
}

void print_question(int player_fd, struct Entry question, int question_number)
{
    char buffer[2048];

    sprintf(buffer, "Question %d: %s\n\nOptions:\n", question_number, question.prompt);

    for (int i = 0; i < 3; i++)
    {
        sprintf(buffer + strlen(buffer), "%d. %s\n", i + 1, question.options[i]);
    }

    write(player_fd, buffer, strlen(buffer));
    write(player_fd, "\nPlease enter your answer (1, 2, or 3): \n", strlen("\nPlease enter your answer (1, 2, or 3): \n"));
}

void game_state(int server_fd, struct Entry *questions, int num_questions, struct Player *players, int num_players)
{
    char buffer[1024];
    int correct_answer;
    int player_answers[MAX_CONNECTIONS];
    int track_answer = 0;
    int question_number = 0;

    for (int i = 0; i < num_questions; i++)
    {
        question_number++;

        int disconnected_player_index = -1;
        for (int i = 0; i < num_players; i++)
        {
            if (recv(players[i].fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0)
            {
                disconnected_player_index = i;
                break;
            }
        }

        if (disconnected_player_index != -1)
        {
            printf("Lost connection!\n");
            for (int i = 0; i < num_players; i++)
            {
                close(players[i].fd);
            }
            close(server_fd);
            return;
        }

        for (int j = 0; j < num_players; j++)
        {
            print_question(players[j].fd, questions[i], question_number);
        }

        server_print_question(server_fd, questions[i], question_number);

        correct_answer = questions[i].answer_idx;
        memset(player_answers, 0, sizeof(player_answers));
        track_answer = 0;

        fd_set read_fds;
        FD_ZERO(&read_fds);
        int max_fd = 0;
        for (int j = 0; j < num_players; j++)
        {
            FD_SET(players[j].fd, &read_fds);
            if (players[j].fd > max_fd)
            {
                max_fd = players[j].fd;
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_players; i++)
        {
            if (FD_ISSET(players[i].fd, &read_fds))
            {
                int bytes_received = recv(players[i].fd, buffer, sizeof(buffer), 0);
                if (bytes_received > 0)
                {
                    int answer = atoi(buffer);
                    player_answers[i] = answer;
                    if (answer > 0)
                    {
                        track_answer++;
                    }
                    printf("\nPlayer %s answered: %d, Correct Answer: %d\n\n", players[i].name, answer, correct_answer);
                }
                else if (bytes_received == 0)
                {
                    printf("Lost connection!\n");
                    for (int j = 0; j < num_players; j++)
                    {
                        close(players[j].fd);
                    }
                    close(server_fd);
                    return;
                }
                else
                {
                    perror("Error receiving message from player");
                    exit(EXIT_FAILURE);
                }
            }
        }

        char *empty = "";
        for (int j = 0; j < num_players; j++)
        {
            // printf("%d\n", correct_answer);
            if (player_answers[j] == atoi(empty))
            {
                write(players[j].fd, "You did not answer!\n", strlen("You did not answer!\n\n"));
            }
            else if (player_answers[j] == correct_answer)
            {
                players[j].score++;
                write(players[j].fd, "You answered correctly! You earned 1 point.\n", strlen("You answered correctly! You earned 1 point.\n"));
            }
            else
            {
                players[j].score--;
                write(players[j].fd, "You answered incorrectly! You lost 1 point.\n", strlen("You answered incorrectly! You lost 1 point.\n"));
            }

            char answer_msg[64];
            sprintf(answer_msg, "The correct answer is: %d\n\n", correct_answer);
            write(players[j].fd, answer_msg, strlen(answer_msg));
        }

        sleep(1);
    }

    int max_score = -9999;
    for (int i = 0; i < num_players; i++)
    {
        if (players[i].score > max_score)
        {
            max_score = players[i].score;
        }
    }

    char winners[1024];
    char message[2048];
    sprintf(winners, "Congrats, ");
    int winner_count = 0;
    for (int i = 0; i < num_players; i++)
    {
        if (players[i].score == max_score)
        {
            if (winner_count > 0)
            {
                strcat(winners, ", ");
            }
            strcat(winners, players[i].name);
            winner_count++;
        }
    }
    strcat(winners, "!\n");
    // strcat(winners, "! You are the winner(s) with ");
    // sprintf(winners + strlen(winners), "%d points!\n", max_score);
    printf("%s", winners);
    for (int i = 0; i < num_players; i++)
    {
        write(players[i].fd, winners, strlen(winners));
    }

    // printf("%s!", winners);

    close(server_fd);
}

int main(int argc, char *argv[])
{
    int opt;
    char *question_file = DEFAULT_QUESTION_FILE;
    char *ip_address = DEFAULT_IP_ADDRESS;
    int port_number = DEFAULT_PORT;

    while ((opt = getopt(argc, argv, ":f:i:p:h")) != -1)
    {
        switch (opt)
        {
        case 'f':
            question_file = optarg;
            break;
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

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
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

        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("Bind failed");
            exit(1);
        }
        else
        {
            if (listen(server_fd, MAX_CONNECTIONS) == -1)
            {
                perror("Listen failed");
                exit(1);
            }
            else
            {
                printf("Welcome to 392 Trivia!\n");

                struct Entry questions[50];
                int num_questions = read_questions(questions, question_file);

                printf("Loaded %d questions from file.\n", num_questions);

                struct Player players[MAX_CONNECTIONS];
                int num_players = 0;

                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(server_fd, &readfds);
                int max_fd = server_fd;

                int names = 0;

                while (1)
                {
                    FD_ZERO(&readfds);
                    FD_SET(server_fd, &readfds);
                    for (int i = 0; i < num_players; i++)
                    {
                        FD_SET(players[i].fd, &readfds);
                    }

                    int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
                    if (activity < 0)
                    {
                        perror("Error in select");
                        exit(1);
                    }

                    if (FD_ISSET(server_fd, &readfds))
                    {
                        int client_fd = accept(server_fd, NULL, NULL);
                        if (client_fd == -1)
                        {
                            perror("Accept failed");
                            continue;
                        }

                        if (num_players < MAX_CONNECTIONS)
                        {
                            players[num_players].fd = client_fd;
                            players[num_players].score = 0;
                            num_players++;
                            printf("New connection detected!\n");
                            write(client_fd, "Please type your name:\n", strlen("Please type your name:\n"));

                            if (client_fd > max_fd)
                            {
                                max_fd = client_fd;
                            }
                        }
                        else
                        {
                            printf("Max connection reached!\n");
                            close(client_fd);
                        }
                    }

                    for (int i = 0; i < MAX_CONNECTIONS; i++)
                    {
                        if (FD_ISSET(players[i].fd, &readfds))
                        {
                            char name_buffer[128];
                            int bytes_received = recv(players[i].fd, name_buffer, sizeof(name_buffer), 0);
                            if (bytes_received > 0)
                            {
                                name_buffer[bytes_received] = '\0';
                                name_buffer[strcspn(name_buffer, "\n")] = '\0';
                                printf("Hi %s!\n", name_buffer);
                                strcpy(players[i].name, name_buffer);
                                names++;
                            }
                            else if (bytes_received == 0)
                            {
                                printf("Client disconnected before entering name.\n");
                                close(players[i].fd);
                                FD_CLR(players[i].fd, &readfds);
                                for (int j = i; j < num_players - 1; j++)
                                {
                                    players[j] = players[j + 1];
                                }
                                num_players--;
                            }
                            else
                            {
                                perror("Error receiving name from client");
                                exit(1);
                            }
                        }
                    }

                    if (names == MAX_CONNECTIONS)
                    {
                        game_state(server_fd, questions, num_questions, players, num_players);
                        break;
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}

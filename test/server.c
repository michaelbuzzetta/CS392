#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int player_count=0;
struct Player {
int fd;
 int score;
char name[128];
};
struct Player player_dict[3];
int count=0;
struct Entry {
 char prompt[1024];
 char options[3][50];
 int answer_idx;
 //implement question amount part

 };
 struct Entry arr[50]; 
 
 void correct_printer(struct Entry *question, int question_number){
    char correct[1024];
    sprintf(correct, "The correct answer was: %s\n", question[question_number].options[question[question_number].answer_idx]);
    printf("%s", correct); 
    for(int x=0; x<count;x++){
        send(player_dict[x].fd,correct,strlen(correct),0);
    }
 }
 void point_allocation(struct Entry *question,int question_number){
    char answers[1024];
    for(int x=0;x<count; x++){
        int player_answer=recv(player_dict[x].fd,answers, sizeof(answers)-1,0);
        if(atoi(answers)==question[question_number].answer_idx +1){
            player_dict[x].score++;
        }
        else{
            player_dict[x].score--;
        }
    }
 }
 void question_printer(struct Entry *question,int question_amount){
    //printf("yoooo");
    for(int x=0; x<question_amount;x++){
        char temp[2000];
        printf("i made it");
        sprintf(temp, "Question %d: %s\n1: %s\n2: %s\n3: %s\n",
        question_amount + 1, question[question_amount].prompt,
        question[question_amount].options[0], question[question_amount].options[1], question[question_amount].options[2]);
        printf("%s", temp);
        for (int i = 0; i <count; i++) {
        send(player_dict[i].fd, temp, strlen(temp), 0); 
    }
    point_allocation(question,x);
    correct_printer(question,x);

    }

 }
void accept_new(int fd,int question_amount){
 
    //struct sockaddr_in client_addr;
    //socklen_t addr_size = sizeof(client_addr);
  
    /*while (player_count < 2) {
        puts("no here");
        int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_size);
        puts("no here here");
        if (client_fd == -1) {
            puts("hi");
            perror("Accept failed");
            continue;
        }

        puts("New connection detected!");
        send(client_fd, "Please type your name:\n", 23, 0);

        char buffer[128];
        int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            strncpy(player_dict[player_count].name, buffer, 127);
            player_dict[player_count].fd = client_fd;
            player_dict[player_count].score = 0;
            printf("Hi %s!\n", player_dict[player_count].name);
            player_count++;
        } else {
            printf("Failed to receive name, closing connection.\n");
            close(client_fd);
        }
    }*/
    int players_in=0;
    int max_fd;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    max_fd=fd;
    for (size_t i = 0; i < 3; i ++) player_dict[i].fd = -1;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        printf("%d\n", fd);
    
        
        int x=0;
        while(x<count){
            //if (player_dict[x].fd > -1)
                FD_SET(player_dict[x].fd,&readfds);
             if (player_dict[x].fd > max_fd) {
            max_fd = player_dict[x].fd;
    }
            x++;
        }
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        /*if((select(max_fd + 1, &readfds, NULL, NULL, NULL))<0){
            puts("here");
            perror("Error in the select function");
            exit(EXIT_FAILURE);
        }*/
       
        if(FD_ISSET(fd,&readfds)){
        int client_fd=accept(fd,NULL,NULL);
        printf("i made it");
        if (count<3){
            puts("New connection detected!\n");
            player_dict[count].fd = client_fd;
            player_dict[count].score = 0;
            write(client_fd, "Please type your name:\n", strlen("Please type your name:\n"));
            if(client_fd>max_fd){
                max_fd=client_fd;
            }
            count++;}
             else{
            puts("Max connections!");
            close(client_fd);
        }}
            for(int x=0;x<3;x++){
                printf("made it to name reading");
           if (FD_ISSET(player_dict[x].fd, &readfds)){
            char buffer[128];
            int bytes_read = recv(player_dict[x].fd, buffer, sizeof(buffer) - 1, 0);
            if(bytes_read>0){
                players_in++;
                buffer[bytes_read] = '\0';
                strcpy(player_dict[count].name, buffer);
                printf("Hi %s!\n", player_dict[count].name);
        }
            else{
                close(player_dict[x].fd);
                puts("Failed to receive name, closing connection.");

            }}
            }
            
            if(count==3){
                printf("im here");
                 question_printer(arr,question_amount);
            }
        }
        //}
        close(fd);
        return;

        



    
}

int read_questions(struct Entry* arr, char* filename){
    puts("hi");
    char temp[1024];
    FILE *questions = fopen(filename, "r");
    int index=0;
    //printf(questions);
    int number_line=0;
    while(fgets(temp, sizeof(temp), questions)) {
    temp[strcspn(temp, "\n")] = '\0';  // Correct way to remove the newline
    number_line++;
    if ((number_line - 1) % 4 == 0) {
        strncpy(arr[index].prompt, temp, sizeof(arr[index].prompt) - 1);
        arr[index].prompt[sizeof(arr[index].prompt) - 1] = '\0'; // Ensure null-termination
    } else if ((number_line - 1) % 4 == 1) {
        char *t = strtok(temp, " ");
        int x = 0;
        while (t != NULL && x < 3) {
            strncpy(arr[index].options[x], t, sizeof(arr[index].options[x]) - 1);
            arr[index].options[x][sizeof(arr[index].options[x]) - 1] = '\0'; // Ensure null-termination
            t = strtok(NULL, " ");
            x++;
        }
    } else if ((number_line - 1) % 4 == 2) {
        for (int x = 0; x < 3; x++) {
            if (strcmp(temp, arr[index].options[x]) == 0) {
                arr[index].answer_idx = x;
            }
        }
    }
    if ((number_line - 1) % 4 == 3) { // Ensure this increment only happens after a full set of question data
        index++;
    }
}
fclose(questions);
        return index;

       

}
int main(int argc, char *argv[]){
int option;
char *question_file= "questions.txt";
    char *ip_address="127.0.0.1";
    int port_number=25555;
while(option= getopt(argc, argv, "f:i:p:h")!=-1){
    
    switch(option){
        case 'f':
            question_file=optarg;
            break;
        case 'i':
            ip_address=optarg;
            break;
        case 'p':
            port_number=atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s [-f question_file] [-i IP_address] [-p port_number] [-h]\n", argv[0]);
            printf("\t-f question_file  Default to \"questions.txt\";\n");
            printf("\t-i IP_address     Default to \"127.0.0.1\";\n");
            printf("\t-p port_number    Default to 25555;\n");
            printf("\t-h                Display this help info.\n");
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
            exit(EXIT_FAILURE);
    }

}
    int    server_fd;
    int    client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in in_addr;
    socklen_t addr_size = sizeof(in_addr);

    /* STEP 1
        Create and set up a socket
    */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(25555);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* STEP 2
        Bind the file descriptor with address structure
        so that clients can find the address
    */
    bind(server_fd,
            (struct sockaddr *) &server_addr,
            sizeof(server_addr));

    /* STEP 3
        Listen to at most 5 incoming connections
    */
    if (listen(server_fd, 3) == 0)
        printf("Listening\n");
    else perror("listen");
    //printf("heyyy");
    puts("Welcome to 392 Trivia!\n");
    //printf("i got here");
    int question_amount=read_questions(arr,question_file);
    printf("%d\n", question_amount);
    printf("%d\n", server_fd);
    accept_new(server_fd,question_amount);
    
    //question_printer(arr);
    
    //for(int x=0; x<question_amount;x++){
        //point_allocation(arr, x);
        //correct_printer(arr,x);
    //}
    int max_index=0;
    for(int x=1;x<count;x++){
        if(player_dict[x].score>player_dict[x-1].score){
            max_index=x;
        }
    }
    char congrats[1024];
    sprintf(congrats, "Congrats, %s!\n", player_dict[max_index].name);
    printf("%s", congrats);
    for(int x=0;x<count;x++){
        close(player_dict[x].fd);
    }
    return 0;
}
//puts("Welcome to 392 Trivia!\n");
//return 0;


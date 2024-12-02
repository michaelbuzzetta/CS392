#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


void game_code(int server_fd){
   int max_fd;
if (server_fd > STDIN_FILENO) {
    max_fd = server_fd;
} else {
    max_fd = STDIN_FILENO;
}
    
    fd_set active_fds;
    //fd_set active_fds_2;
    
    //add part for name
    while(1){
        //active_fds_2=active_fds;
        FD_ZERO(&active_fds);
        FD_SET(STDIN_FILENO, &active_fds);
        FD_SET(server_fd, &active_fds);
        select(max_fd + 1, &active_fds, NULL, NULL, NULL);
        if(FD_ISSET(STDIN_FILENO,&active_fds)){
            //printf("in stdin");
            char buffer[1024];
            fgets(buffer, sizeof(buffer), stdin);
            send(server_fd, buffer, strlen(buffer), 0);
        }
        if(FD_ISSET(server_fd,&active_fds)){
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
    }}
    /*printf("yo im here");
    while(1){
        //may need to add something here is there is a point with no input
    char buffer[1024];
    fflush(stdout);
    scanf("%s",buffer);
    send(server_fd,buffer,strlen(buffer),0);
    int recvbytes=recv(server_fd,buffer,1024,0);
    if(recvbytes==0){
        break;
    }
    else{
        buffer[recvbytes] = 0;
        printf("%s",buffer); 
        fflush(stdout);
    }

    }*/
}
void parse_connect(int argc, char** argv, int* server_fd){
    puts("Client made it here");
    struct sockaddr_in server_addr;
    socklen_t addr_size;

    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(25555);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(*server_fd,(struct sockaddr *) &server_addr,addr_size);
    puts("client has connected");
}
int main(int argc, char **argv) {
    int server_fd;
    int option;
    int port_number=25555;
    char *ip_address="127.0.0.1";
    while ((option = getopt(argc, argv, "i:p:h")) != -1) {
        switch(option){
            case 'i':
                ip_address=optarg;
                break;
            case 'p':
                port_number=atoi(optarg);
                break;
            case 'h':
                printf("Usage: %s [-i IP_address] [-p port_number] [-h]\n", argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("Error: Unknown option '-%c' received.\n", optopt);
                exit(EXIT_FAILURE);
        }}
        parse_connect(argc,argv,&server_fd);
        game_code(server_fd);
        close(server_fd);
        return 0;
        }
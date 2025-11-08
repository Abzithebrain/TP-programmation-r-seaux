#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERVER "127.0.0.1"
#define PORT 1234
#define BUFLEN 512


void stop(char* msg){
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]){

    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        stop("socket()");
    }
    //printf("socket créé\n");

    struct sockaddr_in sockaddr;
    memset(&sockaddr,0,sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = inet_addr(SERVER);
    






}
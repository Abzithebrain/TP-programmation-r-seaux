#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define SERVER "127.0.0.1"
#define PORT 1234

void stop(char*){
    perror("msg");
    exit(1);
}

int main(int argc, char* argv[]){ 
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        stop("socket()");
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); //equivalent bzero(&servaddr, sizeof(servaddr))
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER);

    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){
        stop("bind()");
    }

    


    

    
    
    
}
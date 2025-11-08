#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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
    

    
    
    
}
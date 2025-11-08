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
   // printf("socket créé\n");


    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr)); //equivalent bzero(&servaddr, sizeof(servaddr))
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER);

    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){
        stop("bind()");
    }
   // printf("bind ok\n");


    if(listen(sockfd,5)==-1){
        stop("listen()");
    }
  //  printf("listen ok\n");
    
    int newsockfd,clilen;
    struct sockaddr_in cliaddr;
    memset(&cliaddr,0,sizeof(cliaddr));
    clilen = sizeof(cliaddr);
    if ((newsockfd = accept(sockfd,(struct sockaddr*)&cliaddr,(socklen_t *)&clilen) )< 0)
    {
        stop("accept()");
    }
   // printf("accept ok\n");
    
    char message[BUFLEN+1];
    for (int i = 0; i < 1000; i++)
    {
        memset(&message,0,BUFLEN+1);
        int n = recv(newsockfd,message,BUFLEN,0); 
        if (n < 0)
        {
            stop("recv()");
        }
        

        printf("(%d octets)reçu: %s\n",n,message);
        if (send(newsockfd,message,n,0)==-1)
        {
            stop("send()");
        
        }
      //  printf("envoyé\n");
        
        
    }
    

    close(newsockfd);
    close(sockfd);


    return 0;

    
    
    

    
}
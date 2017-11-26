// Written by Robin Verschueren, used and modified (comments) by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/UDPserver.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>


//_____________________________________________________________________________
UDPserver::UDPserver(int udp_port) {
    
    slen=sizeof(cli_addr);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      err("socket");
    else 
      printf("Server : Socket() successful\n");

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(udp_port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Server : bind() successful\n");

    // setting timeout to 1 sec
    tv.tv_sec=0;
    tv.tv_usec=500;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    printf("setsockopt Error");
    } else printf("TIMEOUT SET %i\n");
    
}

//_____________________________________________________________________________
void UDPserver::listen(char buf[]) {
    
    //if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
        //err("recvfrom()");
    //printf("recvfrom()");
  recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen);
}

//_____________________________________________________________________________
void UDPserver::shutDown() {
    
    close(sockfd);
}

//_____________________________________________________________________________
void UDPserver::err(char const *str) {
    
    perror(str);
    exit(1);
}

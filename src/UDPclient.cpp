// Written by Robin Verschueren, used and modified (comments) by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../include/UDPclient.h"

//_____________________________________________________________________________
UDPclient::UDPclient(int udp_port) {

	slen=sizeof(cli_addr);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		err("socket");
	else
		//printf("Server : Socket() successful\n");

	//if (udp_port == -1) {udp_port = PORTVIS;}

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(udp_port);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

	//printf("Socket client at port %d\n", udp_port);
}

//_____________________________________________________________________________
int UDPclient::send(char buf[]) {

	// dbg
	//printf("sendto port %d\n", cli_addr.sin_port);
	//int n = sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, slen);

	int n = sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&my_addr, slen);
	//printf("UDPclient: n %i\n", n);
	if (n ==-1 )
		err("sendto()");
	return n;
}

// recieve function added  10th Feb 2014
void UDPclient::listen(char buf[]) {

	if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1)
		err("recvfrom()");
}




//_____________________________________________________________________________
void UDPclient::shutDown() {

	close(sockfd);
}

//_____________________________________________________________________________
void UDPclient::err(char const *str) {

	perror(str);
	exit(1);
}

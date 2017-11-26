// Written by Robin Verschueren, used and modified (comments) by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars
// (made the port number customizable by constructor)

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <netinet/in.h>
#include <sys/socket.h>

#define BUFLEN 1024 
#define PORT 9931

// This class implements a UDP socket on localhost for receiving data.
class UDPserver 
{
    public:
        // open port on provided port number
        UDPserver(int udp_server = -1);

        // check for data and write to provided buffer
        void listen(char buf[]);        

        void shutDown();
    
    private:
        void err(char const *str);
        int sockfd;
        struct sockaddr_in my_addr, cli_addr;
        socklen_t slen;
        struct timeval tv;
};

#endif // UDPSERVER_H

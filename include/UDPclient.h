// Written by Robin Verschueren, used and modified by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars
// (made the port number customizable by constructor)

#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <netinet/in.h>
#include <sys/socket.h>

#define BUFLEN 1024 
#define PORTVIS 9930
#define PORTDIS 9931

// This class implements a UDP socket on localhost for sending data.
class UDPclient 
{
    public:
        // Open port on provided port number
        UDPclient(int udp_port = -1);

        void listen(char buf[]);

        // Send c-String to port
        int  send(char buf[]);

        void shutDown();
    
    private:
        void err(char const *str);
        int sockfd;
        struct sockaddr_in my_addr, cli_addr;
        socklen_t slen;
};

#endif // UDPCLIENT_H


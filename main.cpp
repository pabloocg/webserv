/*
                        SERVER
    int sockfd = socket(domain, type, protocol)
sockfd: socket descriptor, an integer (like a file-handle)
domain: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
type: communication type
    SOCK_STREAM: TCP(reliable, connection oriented)
    SOCK_DGRAM: UDP(unreliable, connectionless)
protocol: Protocol value for Internet Protocol(IP), which is 0.
    This is the same number which appears on protocol field in the IP header of a packet.(man protocols for more details)
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
This helps in manipulating options for the socket referred by the file descriptor sockfd.
This is completely optional, but it helps in reuse of address and port.
Prevents error such as: “address already in use
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
After creation of the socket, bind function binds the socket to the address and port number specified in addr(custom data structure).
In the example code, we bind the server to the localhost, hence we use INADDR_ANY to specify the IP address.
    int listen(int sockfd, int backlog);
It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow.
If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED.
    int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
It extracts the first connection request on the queue of pending connections for the listening socket, sockfd,
creates a new connected socket, and returns a new file descriptor referring to that socket.
At this point, connection is established between client and server, and they are ready to transfer data.
*/

// Server side C/C++ program to demonstrate Socket programming
//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <vector>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>	   //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "Server.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8080

int main(){
	http::Server serv;
	serv.start();
	while(1){
		serv.wait_for_connection();
	}
	return 0;
}

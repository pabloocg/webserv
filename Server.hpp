#ifndef HTTP_HPP
#define HTTP_HPP

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



#define TRUE 1
#define FALSE 0
#define PORT 8080

namespace http{

	class Server{
	private:
		typedef struct sockaddr_in SA_IN;

		int server_socket;
		int *client_socket;
		int max_client;
		//char *buffer;
		SA_IN address;
		fd_set readfds;

		

	public:
		Server();
		//~Server();

		void start();
		void wait_for_connection();

};

}


#endif
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
#include "Connection.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define ROOT_DIR "dir"

std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string tok;

	while (std::getline(ss, tok, delimiter))
		internal.push_back(tok);
	return (internal);
}

char *read_file(std::string file_request)
{
	std::ifstream file;
	std::stringstream content_file;
	std::string buf;
	char *res;
	int status;
	std::string message_status;
	std::string file_type;

	//Read the file requested
	//file_request = file_request.substr(1, file_request.size());
	file_type = file_request.substr(file_request.find(".") + 1, file_request.size());
	if (file_request == file_type)
		file_type = "html";
	std::cout << file_type << std::endl;
	std::cout << "******" << file_request << "*******" << std::endl;
	if (file_request == "dir/")
		file_request = "dir/index.html";
	file.open(file_request);
	if (file.is_open())
	{
		status = 200;
		message_status = "OK";
	}
	//Read the error page 404
	else
	{
		file.open("dir/404.html");
		status = 404;
		message_status = "Not Found";
	}
	//Should also do one if the client not have permissions to read the file page 403
	while (std::getline(file, buf))
		content_file << buf << "\n";
	buf = content_file.str();
	content_file.str("");
	content_file.clear();
	file.close();
	content_file << "HTTP/1.1 " << status << " " << message_status << "\nContent-Type: text/" << file_type << "\ncharset=utf-8\nContent-Length: " << buf.length() << "\n\n"
				 << buf;
	buf = content_file.str();
	if (!(res = (char *)malloc(sizeof(char) * (buf.size() + 1))))
		return (NULL);
	std::copy(buf.begin(), buf.end(), res);
	res[buf.size()] = '\0';
	return (res);
}

char *parse_headers(std::string header)
{
	std::vector<std::string> sheader;
	std::vector<std::string> request;

	sheader = split(header, '\n');
	request = split(sheader[0], ' ');
	if (request[0] == "GET")
		return (read_file(ROOT_DIR + request[1]));
	return (NULL);
}

int main(/*int argc, char *argv[]*/)
{
	int master_socket, addrlen, new_socket, client_socket[30],
		max_clients = 30, activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;

	char buffer[30000] = {0}; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message;

	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	Connection connection(PORT);

	master_socket = connection.getSocket();
	addrlen = connection.getAddrlen();
	address = connection.getAddress();

	while (TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			if (sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}
		std::cout << "select " << activity << std::endl;
		if (FD_ISSET(master_socket, &readfds)) //nueva conexion entrante
		{
			if ((new_socket = accept(master_socket,
									 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			valread = read(new_socket, buffer, 30000);

			printf("New connection , socket fd is %d\n", new_socket);
			message = parse_headers(buffer);
			if (!message)
			{
				perror("some error occured");
				exit(EXIT_FAILURE);
			}
			if (send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message))
			{
				perror("send");
			}
			free(message);
			puts("Message sent");
			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					printf("Adding fd %d to list of sockets as %d\n", new_socket, i);
					break;
				}
			}
		}
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			if (FD_ISSET(sd, &readfds))
			{
				if ((valread = read(sd, buffer, 30000)) == 0)
				{
					printf("Host disconnected with fd = %d\n", sd);
					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					message = parse_headers(buffer);
					if (!message)
					{
						perror("some error occured");
						exit(EXIT_FAILURE);
					}
					if (send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message))
					{
						perror("send");
					}
					free(message);
					puts("Message sent");
				}
			}
		}
	}

	return 0;
}

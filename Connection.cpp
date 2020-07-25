#include "Connection.hpp"

Connection::Connection(int port)
{
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	_opt = TRUE;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&_opt,
				   sizeof(_opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);
	if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", port);
	if (listen(_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	_addrlen = sizeof(_address);
	puts("Waiting for connections ...");
}

Connection::~Connection()
{
}

int Connection::getSocket()
{
	return (_socket);
}

int Connection::getAddrlen()
{
	return (_addrlen);
}

struct sockaddr_in Connection::getAddress()
{
	return (_address);
}

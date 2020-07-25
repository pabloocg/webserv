#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>

#define TRUE 1
#define FALSE 0

class Connection
{
private:
	struct sockaddr_in _address;
	int _socket;
	int _opt;
	int _addrlen;

public:
	Connection(int port);
	~Connection();

	int getSocket();
	int getAddrlen();
	struct sockaddr_in getAddress();
};

#endif

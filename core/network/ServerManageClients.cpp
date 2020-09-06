#include "ServerC.hpp"

void	http::ServerC::add_client(int &new_socket)
{
	http::Client	new_client(new_socket);

	this->_clients.push_back(new_client);
	FD_SET(new_socket, &this->_master_read);

#ifdef 		DEBUG_MODE

	std::cout << "New Client " << new_socket << " connected" << std::endl;
	std::cout << "FD list:" << std::endl;
	for (size_t j = 0; j < this->_clients.size(); j++)
	{
		std::cout << j << " : " << this->_clients[j].getFd() << std::endl;
	}

#endif

}

void	http::ServerC::remove_client(std::vector<http::Client>::iterator &client)
{
	int		sd;

	sd = client->getFd();
	close(client->getFd());
	FD_CLR(client->getFd(), &_master_read);
	FD_CLR(client->getFd(), &_master_write);
	this->_clients.erase(client);

#ifdef 		DEBUG_MODE

	std::cout << "Host number " << sd << " disconnected" << std::endl;
	std::cout << "FD list:" << std::endl;
	for (size_t j = 0; j < this->_clients.size(); j++)
	{
		std::cout << j << " : " << this->_clients[j].getFd() << std::endl;
	}

#endif
}

void http::ServerC::accept_connection(SA_IN &address, int i)
{

#ifdef 		DEBUG_MODE

	std::cout << "New connection at server " << i << "!" << std::endl;

#endif

	int new_socket;
	int addrlen;

	addrlen = sizeof(address);
	if ((new_socket = accept(this->_server_socket[i], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		throw ServerError("accept", "failed for some reason");
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerError("fcntl", "failed for some reason");
	this->add_client(new_socket);
}

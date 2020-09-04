#include "ServerC.hpp"

void	http::ServerC::add_client(int &new_socket)
{
	for (int j = 0; j < _max_client; j++)
	{
		if (this->_client_socket[j] == 0)
		{
			this->_client_socket[j] = new_socket;
			FD_SET(new_socket, &this->_master_read);

#ifdef 		DEBUG_MODE

			std::cout << "Host number " << j << " connected" << std::endl;
			std::cout << "FD list:" << std::endl;
			for (int k = 0; k < this->_max_client; k++)
				if (this->_client_socket[k])
					std::cout << k << " : " << this->_client_socket[k] << std::endl;

#endif
			break;
		}
	}
}

void	http::ServerC::remove_client(int &sd, int j)
{
	close(sd);
	FD_CLR(sd, &_master_read);
	_client_socket[j] = 0;

#ifdef 		DEBUG_MODE

	std::cout << "Host number " << sd << " disconnected" << std::endl;
	std::cout << "FD list:" << std::endl;
	for (int k = 0; k < this->_max_client; k++)
		if (this->_client_socket[k])
			std::cout << k << " : " << this->_client_socket[k] << std::endl;

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

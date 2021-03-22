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

std::vector<http::Client>::iterator	http::ServerC::remove_client(std::vector<http::Client>::iterator &client)
{
	int		sd;
	std::vector<http::Client>::iterator	tmp;

	sd = client->getFd();
	close(sd);
	if (FD_ISSET(sd, &_master_read))
		FD_CLR(sd, &_master_read);
	if (FD_ISSET(sd, &_master_write))
		FD_CLR(sd, &_master_write);
	tmp = this->_clients.erase(client);

#ifdef 		DEBUG_MODE

	std::cout << "Client number " << sd << " disconnected" << std::endl;
	std::cout << "FD list:" << std::endl;
	for (size_t j = 0; j < this->_clients.size(); j++)
		std::cout << j << " : " << this->_clients[j].getFd() << std::endl;

#endif
	return (tmp);
}

void	http::ServerC::remove_tmp_client(http::Client &client)
{
	int		sd;

	sd = client.getFd();
	close(sd);
	if (FD_ISSET(sd, &_master_write))
		FD_CLR(sd, &_master_write);
}

void http::ServerC::accept_connection(http::ServerConf &server, int &server_socket)
{
	int			new_socket = -1;
	SA_IN		address;
	socklen_t	addrlen;

	address = server.getInfoAddress();
	addrlen = sizeof(struct sockaddr);
	if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		throw ServerError("accept", "failed for some reason");
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerError("fcntl", "failed for some reason");
	this->add_client(new_socket);
}

void	http::ServerC::reject_connection(http::ServerConf &server, int &server_socket)
{
	int 			new_socket = -1;
	int 			valwrite;
	SA_IN			address;
	socklen_t		addrlen;
	http::Request	req(503);
	ssize_t			size;
	char			*message;

	address = server.getInfoAddress();
	addrlen = sizeof(struct sockaddr);
	if ((new_socket = accept(server_socket, (struct sockaddr *)&address, &addrlen)) == -1)
		throw ServerError("accept", "failed for some reason");
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
		throw ServerError("fcntl", "failed for some reason");
	
	http::Client	tmp_client(new_socket);
	if (!(message = req.build_response(&size, _mime_types)))
		throw ServerError("request", "failed for some reason");
	tmp_client.setSending(true);
	tmp_client.setupSend(message, size, 0, size);
	if ((valwrite = send(tmp_client.getFd(), tmp_client.getSendMessage() + tmp_client.getSended(), tmp_client.getSendLeft(), 0)) < 0)
	{
		free(tmp_client.getSendMessage());
		this->remove_tmp_client(tmp_client);
	}
	else
	{
		tmp_client.setSending(false);
		free(tmp_client.getSendMessage());
		this->remove_tmp_client(tmp_client);
	}
	/*
	if ((int)this->_tmp_clients.size() > MAX_TMP_CLIENTS)
		close(new_socket);
	else
	{
		http::Client	new_client(new_socket);
		this->_tmp_clients.push(new_client);
		FD_SET(new_socket, &this->_master_write);
	}
	*/
}

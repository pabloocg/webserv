#include "ServerC.hpp"

void http::ServerC::wait_for_connection()
{
	int max_sd, sd, activity;
	fd_set readfds;
	fd_set writefds;
	SA_IN address;

	max_sd = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	readfds = _master_read;
	writefds = _master_write;
	for (int i = 0; i < (int)_server_socket.size(); i++)
		if (_server_socket[i] > max_sd)
			max_sd = _server_socket[i];
	for (size_t i = 0; i < this->_clients.size(); i++)
		if (this->_clients[i].getFd() > max_sd)
			max_sd = this->_clients[i].getFd();

#ifdef DEBUG_MODE

	std::cout << "Waiting for select" << std::endl;

#endif

	if (((activity = select(max_sd + 1, &readfds, &writefds, NULL, NULL)) < 0) && (errno != EINTR))
		throw ServerError("select", "failed for some reason");

#ifdef DEBUG_MODE

	if (activity > 0)
		std::cout << "Number of reads/writes possible " << activity << std::endl;

#endif

	for (size_t i = 0; i < _servers.size(); i++)
	{
		address = _servers[i].getInfoAddress();
		if (FD_ISSET(_server_socket[i], &readfds))
			this->accept_connection(address, i);
	}
	std::vector<http::Client>::iterator client = this->_clients.begin();
	std::vector<http::Client>::iterator client_end = this->_clients.end();
	for (; client != client_end; client++)
	{
		sd = client->getFd();
		if (FD_ISSET(sd, &readfds))
		{
			int valread;
			char buffer[BUFFER_SIZE + 1] = {0};

			if ((valread = read(sd, buffer, BUFFER_SIZE)) <= 0)
			{
				//throw ServerError("read", "failed for some reason");
				this->remove_client(client);
				continue;
			}
			//else if (!valread)
				//this->remove_client(client);
			else
			{
				buffer[valread] = '\0';
				read_request(buffer, client);
			}
		}
		if (FD_ISSET(sd, &writefds))
		{
			int valwrite;

			if ((valwrite = send(sd, client->getSendMessage() + client->getSended(), client->getSendLeft(), 0)) < 0)
			{
				//throw ServerError("send", "failed for some reason");
				this->remove_client(client);
				continue;
			}
			if (valwrite < client->getSendLeft())
			{
				client->setSended(client->getSended() + valwrite);
				client->setSendLeft(client->getSendLeft() - valwrite);
			}
			else
			{
				client->setSending(false);
				free(client->getSendMessage());
				client->reset_send();
				FD_CLR(sd, &_master_write);
			}

#ifdef DEBUG_MODE

		std::cout << "Sended " << valwrite + client->getSended() << " bytes to " << sd << ", " << client->getSendLeft() - valwrite << " left" << std::endl;

#endif
		}
	}
}

#include "ServerC.hpp"

void http::ServerC::wait_for_connection()
{
	int max_sd, sd, activity;
	fd_set readfds;
	fd_set writefds;

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
		if (FD_ISSET(_server_socket[i], &readfds))
		{

#ifdef DEBUG_MODE
			std::cout << "New connection at server " << i << "!" << std::endl;
#endif

			if ((int)this->_clients.size() > MAX_CLIENTS)
				this->reject_connection(_servers[i], _server_socket[i]);
			else
				this->accept_connection(_servers[i], _server_socket[i]);
		}
	}
	std::vector<http::Client>::iterator client = this->_clients.begin();
	std::vector<http::Client>::iterator client_end = this->_clients.end();
#ifdef DEBUG_MODE
	std::cout << "Actual clients " << this->_clients.size() << std::endl;
#endif
	for (; client != client_end; client++)
	{
		sd = client->getFd();
		if (FD_ISSET(sd, &readfds))
		{
			int valread;
			char buffer[BUFFER_SIZE + 1] = {0};

			if ((valread = read(sd, buffer, BUFFER_SIZE)) <= 0)
			{
				this->remove_client(client);
				continue;
			}
			else
			{
				buffer[valread] = '\0';
				read_request(buffer, client, valread);
			}
		}
		if (FD_ISSET(sd, &writefds))
		{
			int valwrite;

			if ((valwrite = send(sd, client->getSendMessage() + client->getSended(), client->getSendLeft(), 0)) < 0)
			{
				free(client->getSendMessage());
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

#ifdef DEBUG_MODE
			std::cout << "Actual tmp clients " << this->_tmp_clients.size() << std::endl;
#endif
	http::Client	tmp_client;
	while (this->_tmp_clients.size() > 0)
	{
		tmp_client = this->_tmp_clients.front();
		sd = tmp_client.getFd();
		if (FD_ISSET(sd, &_master_write))
		{
			int valwrite;
			if (!tmp_client.isSending())
			{
				http::Request	req(503);
				ssize_t			size;
				char			*message;

				if (!(message = req.build_response(&size, _mime_types)))
					throw ServerError("request", "failed for some reason");
				tmp_client.setSending(true);
				tmp_client.setupSend(message, size, 0, size);
			}
			if ((valwrite = send(sd, tmp_client.getSendMessage() + tmp_client.getSended(), tmp_client.getSendLeft(), 0)) < 0)
			{
				free(tmp_client.getSendMessage());
				this->remove_tmp_client(tmp_client);
				continue;
			}
			if (valwrite < tmp_client.getSendLeft())
			{
				tmp_client.setSended(tmp_client.getSended() + valwrite);
				tmp_client.setSendLeft(tmp_client.getSendLeft() - valwrite);
			}
			else
			{
				tmp_client.setSending(false);
				free(tmp_client.getSendMessage());
				this->remove_tmp_client(tmp_client);
			}

#ifdef DEBUG_MODE

			std::cout << "Sended 503 Error " << valwrite + tmp_client.getSended() << " bytes to " << sd << ", " << tmp_client.getSendLeft() - valwrite << " left" << std::endl;

#endif
		}
		this->_tmp_clients.pop();
	}
}

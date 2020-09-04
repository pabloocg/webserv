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
	for (int i = 0; i < _max_client; i++)
		if (_client_socket[i] > max_sd)
			max_sd = _client_socket[i];

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
	for (int j = 0; j < _max_client; j++)
	{
		sd = _client_socket[j];
		if (FD_ISSET(sd, &readfds))
		{
			int valread;
			char buffer[BUFFER_SIZE + 1] = {0};

			if ((valread = read(sd, buffer, BUFFER_SIZE)) < 0)
				throw ServerError("read", "failed for some reason");
			else if (!valread)
				this->remove_client(sd, j);
			else
			{
				buffer[valread] = '\0';
				read_request(buffer, sd);
			}
		}
		if (FD_ISSET(sd, &writefds))
		{
			int n;
			http::Pending_send pending;

			pending = _pending_messages.find(sd)->second;
			if ((n = send(sd, pending.get_message() + pending.get_sended(), pending.get_left(), 0)) < 0)
				throw ServerError("send", "failed for some reason");
			if (n < pending.get_left())
			{
				_pending_messages.find(sd)->second.set_sended(pending.get_sended() + n);
				_pending_messages.find(sd)->second.set_left(pending.get_left() - n);
			}
			else
			{
				free(pending.get_message());
				FD_CLR(sd, &_master_write);
				_pending_messages.erase(_pending_messages.find(sd));
			}

#ifdef DEBUG_MODE

			std::cout << "Sended " << n + pending.get_sended() << " bytes to " << sd << ", " << pending.get_left() - n << " left" << std::endl;

#endif
		}
	}
}

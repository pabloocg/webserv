#include "ServerC.hpp"

http::ServerC::ServerC() : _client_socket(30, 0),
						   _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
	this->_max_client = 30;
}

http::ServerC::ServerC(std::vector<http::ServerConf> servers, std::map<std::string, std::string> mime_types) : _client_socket(30, 0),
																											   _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG),
																											   _servers(servers),
																											   _mime_types(mime_types)
{
	this->_max_client = 30;
	this->_server_socket.resize(this->_servers.size());
}

void http::ServerC::start()
{
	FD_ZERO(&_master_read);
	FD_ZERO(&_master_write);
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if ((_server_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			this->_log.makeLog(ERROR_LOG, "socket failed");
			exit(EXIT_FAILURE);
		}
		int _opt = TRUE;
		if (setsockopt(_server_socket[i], SOL_SOCKET, SO_REUSEADDR, (char *)&_opt,
					   sizeof(_opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		if (bind(_server_socket[i], (struct sockaddr *)&_servers[i].getInfoAddress(), sizeof(_servers[i].getInfoAddress())) < 0)
		{
			perror("bind failed");
			this->_log.makeLog(ERROR_LOG, "bind failed");
			exit(EXIT_FAILURE);
		}
		std::cout << "Listener on port " << _servers[i].getPort() << std::endl;
		if (listen(_server_socket[i], 256) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if (fcntl(_server_socket[i], F_SETFL, O_NONBLOCK) < 0)
		{
			perror("fcntl");
			exit(EXIT_FAILURE);
		}
		FD_SET(_server_socket[i], &_master_read);
		std::cout << "Server fd is " << _server_socket[i] << std::endl;
		std::cout << "Waiting for connections..." << std::endl;
	}
}

void http::ServerC::wait_for_connection()
{
	int max_sd, sd, activity, new_socket, valread, size;
	char buffer[30000] = {0};
	char *message;
	fd_set readfds;
	FD_ZERO(&readfds);
	fd_set writefds;
	FD_ZERO(&writefds);
	readfds = _master_read;
	writefds = _master_write;
	max_sd = _server_socket[1];
	for (int i = 0; i < _max_client; i++)
		if (_client_socket[i] > max_sd)
			max_sd = _client_socket[i];
	std::cout << "Waiting for select" << std::endl;
	activity = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
	if (activity > 0)
		std::cout << "Number of reads/writes possible " << activity << std::endl;
	if ((activity < 0) && (errno != EINTR))
	{
		printf("select error");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < _servers.size(); i++)
	{
		SA_IN address;

		address = _servers[i].getInfoAddress();
		int addrlen = sizeof(address);
		if (FD_ISSET(_server_socket[i], &readfds)) //nueva conexion entrante
		{
			std::cout << "New connection at server " << i << "!" << std::endl;
			if ((new_socket = accept(_server_socket[i],
									 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			fcntl(new_socket, F_SETFL, O_NONBLOCK);
			for (int j = 0; j < _max_client; j++)
			{
				if (_client_socket[j] == 0)
				{
					_client_socket[j] = new_socket;
					FD_SET(new_socket, &_master_read);
					std::cout << "host number " << j << " connected" << std::endl;
					std::cout << "fd list:" << std::endl;
					for (int k = 0; k < 30; k++)
					{
						if (_client_socket[k])
							std::cout << k << " : " << _client_socket[k] << std::endl;
					}
					break;
				}
			}
		}
	}
	for (int j = 0; j < _max_client; j++)
	{
		sd = _client_socket[j];
		if (FD_ISSET(sd, &readfds))
		{
			if ((valread = read(sd, buffer, 30000)) == 0)
			{
				std::cout << "host number " << j << " disconnected" << std::endl;
				close(sd);
				FD_CLR(_client_socket[j], &_master_read);
				_client_socket[j] = 0;
				std::cout << "fd list:" << std::endl;
				for (int k = 0; k < 30; k++)
				{
					if (_client_socket[k])
						std::cout << k << " : " << _client_socket[k] << std::endl;
				}
			}
			else if (valread == -1)
			{
				perror("read");
			}
			else
			{

				this->_log.makeLog(ACCESS_LOG, buffer);
				http::Request req(buffer);
				message = req.build_response(&size, _mime_types);
				std::cout << "size of the message: " << size << std::endl;
				if (!message)
				{
					perror("some error occured");
					exit(EXIT_FAILURE);
				}
				int n = 0;
				n = send(sd, message, (size_t)size, 0);
				std::cout << "Sended " << n << " bytes to " << sd << ", " << size - n << " left" << std::endl; 
				if (n == -1)
				{
					perror("send");
				}
				else if (n < size){
					Pending_send sended(message, size, n, size - n);
					std::pair<int, http::Pending_send> pair(sd, sended);
					_pending_messages.insert(pair);
					FD_SET(sd, &_master_write);
				}
				else
					free(message);
				std::cout << "Message sent to " << sd << std::endl;
			}
		}
	}
	for (int j = 0; j < _max_client; j++)
	{
		sd = _client_socket[j];
		if (FD_ISSET(sd, &writefds)){
			Pending_send pending = _pending_messages.find(sd)->second;
			int n = 0;
			n = send(sd, pending.get_message() + pending.get_sended(), pending.get_left(), 0);
			std::cout << "Sended " << n + pending.get_sended() << " bytes to " << sd << ", " << pending.get_left() - n << " left" << std::endl; 
			if (n == -1){
				perror("send");
			}
			else if (n < pending.get_left()){
				_pending_messages.find(sd)->second.set_sended(pending.get_sended() + n);
				_pending_messages.find(sd)->second.set_left(pending.get_left() - n);
			}
			else{
				free(pending.get_message());
				FD_CLR(sd, &_master_write);
				_pending_messages.erase(_pending_messages.find(sd));
			}
		}
	}
}
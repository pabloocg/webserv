#include "ServerC.hpp"

http::ServerC::ServerC() : _client_socket(30, 0),
						   _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
	this->_max_client = 30;
}

http::ServerC::ServerC(std::vector<http::ServerConf> servers) : _client_socket(30, 0),
																_log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG),
																_servers(servers)
{
	this->_max_client = 30;
	this->_server_socket.resize(this->_servers.size());
}

void http::ServerC::start()
{
	FD_ZERO(&_master);
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
		FD_SET(_server_socket[i], &_master);
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
	//fd_set writefds;
	//FD_ZERO(&writefds); todavia no se como funciona el select para writes (creo que es para los multiples servidores)
	readfds = _master;
	//writefds = _master[i];
	max_sd = _server_socket[1];
	for (int i = 0; i < _max_client; i++)
		if (_client_socket[i] > max_sd)
			max_sd = _client_socket[i];
	std::cout << "Waiting for select" << std::endl;
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
	if (activity > 0)
		std::cout << "Number of reads possible " << activity << std::endl;
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
					FD_SET(new_socket, &_master);
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
				FD_CLR(_client_socket[j], &_master);
				_client_socket[j] = 0;
				std::cout << "fd list:" << std::endl;
				for (int k = 0; k < 30; k++)
				{
					if (_client_socket[k])
						std::cout << k << " : " << _client_socket[k] << std::endl;
				}
			}
			else
			{
				this->_log.makeLog(ACCESS_LOG, buffer);
				http::Request req(buffer);
				message = req.build_response(&size);
				if (!message)
				{
					perror("some error occured");
					exit(EXIT_FAILURE);
				}
				if (send(sd, message, size, 0) != (ssize_t)size)
				{
					perror("send");
				}
				free(message);
				std::cout << "message sent to " << sd << std::endl;
			}
		}
	}
}
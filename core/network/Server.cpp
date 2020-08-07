#include "Server.hpp"

http::Server::Server() : client_socket(30, 0),
						 _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
	this->max_client = 30;
}

void http::Server::start()
{
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		this->_log.makeLog(ERROR_LOG, "socket failed");
		exit(EXIT_FAILURE);
	}
	int _opt = TRUE;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&_opt,
				   sizeof(_opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(DEFAULT_PORT);
	if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		this->_log.makeLog(ERROR_LOG, "bind failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "Listener on port " << DEFAULT_PORT << std::endl;
	if (listen(server_socket, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	FD_ZERO(&master);
	FD_SET(server_socket, &master);
	std::cout << "Server fd is " << server_socket << std::endl;
	std::cout << "Waiting for connections..." << std::endl;
}

void http::Server::wait_for_connection()
{
	int max_sd, sd, activity, new_socket, valread;
	char buffer[30000] = {0};
	char *message;
	int addrlen = sizeof(address);
	fd_set readfds;
	//fd_set writefds;
	FD_ZERO(&readfds);
	//FD_ZERO(&writefds); todavia no se como funciona el select para writes (creo que es para los multiples servidores)
	readfds = master;
	//writefds = master;
	max_sd = server_socket;
	for (int i = 0; i < max_client; i++)
		if (client_socket[i] > max_sd)
			max_sd = client_socket[i];
	std::cout << "Waiting for select" << std::endl;
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
	if (activity > 0){
		std::cout << "Number of reads possible " << activity << std::endl;
	}
	if ((activity < 0) && (errno != EINTR))
	{
		printf("select error");
	}
	if (FD_ISSET(server_socket, &readfds)) //nueva conexion entrante
	{
		if ((new_socket = accept(server_socket,
								 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		std::cout << "New connection!" << std::endl;
		for (int i = 0; i < max_client; i++)
		{
			if (client_socket[i] == 0)
			{
				client_socket[i] = new_socket;
				FD_SET(new_socket, &master);
				std::cout << "host number " << i << " connected" << std::endl;
				std::cout << "fd list:" << std::endl;
				for (int j = 0; j < 30; j++)
				{
					if (client_socket[j])
						std::cout << j << " : " << client_socket[j] << std::endl;
				}
				break;
			}
		}
	}
	for (int i = 0; i < max_client; i++)
	{
		sd = client_socket[i];
		if (FD_ISSET(sd, &readfds))
		{
			if ((valread = read(sd, buffer, 30000)) == 0)
			{
				std::cout << "host number " << i << " disconnected" << std::endl;
				close(sd);
				FD_CLR(client_socket[i], &master);
				client_socket[i] = 0;
				std::cout << "fd list:" << std::endl;
				for (int j = 0; j < 30; j++)
				{
					if (client_socket[j])
						std::cout << j << " : "  << client_socket[j] << std::endl;
				}
			}
			else
			{
				this->_log.makeLog(ACCESS_LOG, buffer);
				http::Request req(buffer);
				int size;
				message = req.build_response(&size);
				//message = http::parse_headers(buffer);
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
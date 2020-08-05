#include "Server.hpp"

http::Server::Server():
		max_client(30),
		_log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
	//aqui tendriamos que leer el archivo de configuracion y poner los parametros en nuestra clase
	this->client_socket = (int *)calloc(30, sizeof(int));
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
	if (listen(server_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	FD_ZERO(&master);
	FD_SET(server_socket, &master);
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
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
	if ((activity < 0) && (errno != EINTR))
	{
		printf("select error");
	}
	//std::cout << "select " << activity << std::endl;
	if (FD_ISSET(server_socket, &readfds)) //nueva conexion entrante
	{
		if ((new_socket = accept(server_socket,
								 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read(new_socket, buffer, 30000);

		printf("New connection , socket fd is %d\n", new_socket);
		this->_log.makeLog(ACCESS_LOG, buffer);
		http::Request req(buffer);
		message = req.build_response();
		if (!message)
		{
			perror("some error occured");
			exit(EXIT_FAILURE);
		}
		if (send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message))
		{
			perror("send");
			exit(EXIT_FAILURE);
		}
		free(message);
		puts("Message sent");
		for (int i = 0; i < max_client; i++)
		{
			if (client_socket[i] == 0)
			{
				client_socket[i] = new_socket;
				FD_SET(new_socket, &master);
				printf("Adding fd %d to list of sockets as %d\n", new_socket, i);
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
				printf("Host disconnected with fd = %d\n", sd);
				close(sd);
				FD_CLR(client_socket[i], &master);
				client_socket[i] = 0;
			}
			else
			{
				this->_log.makeLog(ACCESS_LOG, buffer);
				http::Request req(buffer);
				message = req.build_response();
				//message = http::parse_headers(buffer);
				if (!message)
				{
					perror("some error occured");
					exit(EXIT_FAILURE);
				}
				if (send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message))
				{
					perror("send");
				}
				free(message);
				puts("Message sent");
			}
		}
	}
}
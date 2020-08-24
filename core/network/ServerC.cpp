#include "ServerC.hpp"

http::ServerC::ServerC(std::vector<http::ServerConf> servers, std::map<std::string, std::string> mime_types, char **env) : _client_socket(30, 0),
																											   _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG),
																											   _servers(servers),
																											   _mime_types(mime_types)
{
	this->_max_client = 30;
	this->_server_socket.resize(this->_servers.size());
	this->_env = http::charptrptrToVector(env);
	for (
		struct {std::vector<http::ServerConf>::iterator it; int i; } v = {this->_servers.begin(), 0}; v.it != this->_servers.end(); v.it++, v.i++)
	{
		std::cout << "SERVER " << v.i << std::endl;
		std::cout << *v.it << std::endl;
		std::vector<http::Routes> routes(v.it->getRoutes());
		for (std::vector<http::Routes>::iterator it = routes.begin(); it != routes.end(); it++)
		{
			std::cout << "ROUTE " << std::endl;
			std::cout << *it << std::endl;
		}
	}
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
	int max_sd, sd, activity;
	fd_set readfds;
	fd_set writefds;
	SA_IN address;

	FD_ZERO(&readfds);
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
		address = _servers[i].getInfoAddress();
		if (FD_ISSET(_server_socket[i], &readfds)) //nueva conexion entrante
		{
			std::cout << "New connection at server " << i << "!" << std::endl;
			int new_socket;
			int addrlen;

			addrlen = sizeof(address);
			if ((new_socket = accept(_server_socket[i],
									 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			fcntl(new_socket, F_SETFL, O_NONBLOCK);
			_client_server_map[new_socket] = i;
			for (int j = 0; j < _max_client; j++)
			{
				if (_client_socket[j] == 0)
				{
					_client_socket[j] = new_socket;
					FD_SET(new_socket, &_master_read);
					std::cout << "Host number " << j << " connected" << std::endl;
					std::cout << "FD list:" << std::endl;
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
			int valread;
			int size;
			char buffer[30000] = {0};
			char *message;

			if ((valread = read(sd, buffer, 30000)) == 0)
			{
				std::cout << "Host number " << sd << " disconnected" << std::endl;
				close(sd);
				FD_CLR(sd, &_master_read);
				_client_socket[j] = 0;
				std::cout << "FD list:" << std::endl;
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
				buffer[valread] = '\0';
				std::string s_buffer = "";
				for (int i = 0; i < valread; i++)
				{
					s_buffer += buffer[i];
				}
				if (_pending_reads.find(sd) != _pending_reads.end())
				{
					_pending_reads[sd] += s_buffer;
				}
				else
				{
					_pending_reads[sd] = s_buffer;
				}
				if (valid_req_format(_pending_reads[sd]))
				{
					this->_log.makeLog(ACCESS_LOG, _pending_reads[sd]);
					http::Request req(_pending_reads[sd], get_server(), this->_bad_request, this->_env);
					message = req.build_response(&size, _mime_types);
					_pending_reads.erase(sd);
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
					else if (n < size)
					{
						http::Pending_send sended(message, size, n, size - n);
						std::pair<int, http::Pending_send> pair(sd, sended);
						_pending_messages.insert(pair);
						FD_SET(sd, &_master_write);
					}
					else
						free(message);
				}
			}
		}

		else if (FD_ISSET(sd, &writefds))
		{
			http::Pending_send pending = _pending_messages.find(sd)->second;
			int n = 0;
			n = send(sd, pending.get_message() + pending.get_sended(), pending.get_left(), 0);
			std::cout << "Sended " << n + pending.get_sended() << " bytes to " << sd << ", " << pending.get_left() - n << " left" << std::endl;
			if (n == -1)
			{
				perror("send");
			}
			else if (n < pending.get_left())
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
		}
	}
}
bool http::ServerC::valid_req_format(std::string buffer)
{
	std::vector<std::string> splitted_req;
	int body_size = 0;
	this->_host_header = "NULL";
	this->_bad_request = false;
	splitted_req = http::split(buffer, '\n');
	if (splitted_req.size() < 2)
	{
		return (false);
	}
	for (int i = 0; i < (int)splitted_req.size(); i++)
	{
		if (splitted_req[i].find("Content-Length:") != std::string::npos)
		{
			body_size = std::atoi(splitted_req[i].substr(16, splitted_req[i].length() - 16).c_str());
		}
		if (splitted_req[i].find("Host:") != std::string::npos){
			if (this->_host_header == "NULL"){
			this->_host_header = splitted_req[i].substr(6, splitted_req[i].length() - 7);
			}
			else
				this->_bad_request = true;
		}
	}
	for (int i = 0; i < (int)buffer.length(); i++)
	{
		if (i == (int)buffer.length() - 1)
		{
			return (false);
		}
		if (buffer[i] == '\n' && buffer[i + 2] == '\n')
		{
			if (i + 3 + body_size == (int)buffer.length() || i + 3 + body_size == (int)buffer.length() - 2)
			{
				if (this->_host_header == "NULL")
					this->_bad_request = true;
				return (true);
			}
			break;
		}
	}
	return (false);
}

http::ServerConf http::ServerC::get_server(void){
	for(int i = 0; i < (int)this->_servers.size(); i++){
		std::vector<std::string> names = this->_servers[i].get_server_host_names();
		for (int j = 0; j < (int)names.size(); j++){
			if (this->_host_header == names[j]){
				return (this->_servers[i]);
			}
		}
	}
	return (get_default_server());
}

http::ServerConf http::ServerC::get_default_server(void){
	for(int i = 0; i < (int)this->_servers.size(); i++){
		if (this->_servers[i].isDefault()){
			return (this->_servers[i]);
		}
	}
	return (this->_servers[0]);
}
/*
void http::ServerC::manage_new_connection(int *server_sckt, SA_IN address, int serv_num)
{
	int new_socket;
	int addrlen;

	addrlen = sizeof(address);
	if ((new_socket = accept(*server_sckt,
							 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	std::cout << "with fd value " << new_socket << std::endl;
	fcntl(new_socket, F_SETFL, O_NONBLOCK);
	_client_server_map[new_socket] = serv_num;
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

void http::ServerC::manage_reads(int &sd)
{
	std::cout << "into read" << std::endl;
	int valread;
	int size;
	char buffer[30000] = {0};
	char *message;

	if ((valread = read(sd, buffer, 30000)) == 0)
	{
		std::cout << "host number " << sd << " disconnected" << std::endl;
		close(sd);
		FD_CLR(sd, &_master_read);
		sd = 0;
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
		std::cout << "build response" << std::endl;
		message = req.build_response(&size, _mime_types, _servers[_client_server_map[sd]]);
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
		else if (n < size)
		{
			http::Pending_send sended(message, size, n, size - n);
			std::pair<int, http::Pending_send> pair(sd, sended);
			_pending_messages.insert(pair);
			FD_SET(sd, &_master_write);
		}
		else
			free(message);
		std::cout << "Message sent to " << sd << std::endl;
	}
}

void http::ServerC::manage_writes(int &sd)
{
	http::Pending_send pending = _pending_messages.find(sd)->second;
	int n = 0;
	n = send(sd, pending.get_message() + pending.get_sended(), pending.get_left(), 0);
	std::cout << "Sended " << n + pending.get_sended() << " bytes to " << sd << ", " << pending.get_left() - n << " left" << std::endl;
	if (n == -1)
	{
		perror("send");
	}
	else if (n < pending.get_left())
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
}
*/

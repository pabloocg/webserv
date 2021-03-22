#include "ServerC.hpp"

http::ServerC::ServerC(std::vector<http::ServerConf> servers,
                        std::map<std::string, std::string> mime_types, char **env): _servers(servers),
                                                                                    _mime_types(mime_types)
{
	this->_server_socket.resize(this->_servers.size());
	this->_env = http::charptrptrToVector(env);

#ifdef 		DEBUG_MODE

	for (struct {std::vector<http::ServerConf>::iterator it; int i; } v = {this->_servers.begin(), 0}; v.it != this->_servers.end(); v.it++, v.i++)
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

#endif
}

void http::ServerC::start()
{
	FD_ZERO(&_master_read);
	FD_ZERO(&_master_write);
	for (size_t i = 0; i < _servers.size(); i++)
	{
		if ((_server_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			throw ServerError("socket", "failed for some reason");
		int _opt = TRUE;
		if (setsockopt(_server_socket[i], SOL_SOCKET, SO_REUSEADDR, (char *)&_opt, sizeof(_opt)) < 0)
			throw ServerError("setsockopt", "failed for some reason");
		if (bind(_server_socket[i], (struct sockaddr *)&_servers[i].getInfoAddress(), sizeof(_servers[i].getInfoAddress())) < 0)
			throw ServerError("bind", "failed for some reason");
#ifdef DEBUG_MODE
		std::cout << "Listener on port " << _servers[i].getPort() << std::endl;
#endif
		if (listen(_server_socket[i], 256) < 0)
			throw ServerError("listen", "failed for some reason");
		if (fcntl(_server_socket[i], F_SETFL, O_NONBLOCK) < 0)
			throw ServerError("fcntl", "failed for some reason");
		FD_SET(_server_socket[i], &_master_read);
#ifdef DEBUG_MODE
		std::cout << "Server fd is " << _server_socket[i] << std::endl;
		std::cout << "Waiting for connections..." << std::endl;
#endif
	}
}

http::ServerConf http::ServerC::get_server(void)
{
	for (int i = 0; i < (int)this->_servers.size(); i++)
	{
		std::vector<std::string> names = this->_servers[i].get_server_host_names();
		for (int j = 0; j < (int)names.size(); j++)
			if (this->_host_header == names[j])
				return (this->_servers[i]);
	}
	return (get_default_server());
}

http::ServerConf http::ServerC::get_default_server(void)
{
	for (int i = 0; i < (int)this->_servers.size(); i++)
		if (this->_servers[i].isDefault())
			return (this->_servers[i]);
	return (this->_servers[0]);
}

http::ServerC::ServerError::ServerError(void)
{
	this->_error = "Undefined Server Exception";
}

http::ServerC::ServerError::ServerError(std::string function, std::string error)
{
	this->_error = function + ": " + error;
}

const char *http::ServerC::ServerError::what(void) const throw()
{
	return (this->_error.c_str());
}

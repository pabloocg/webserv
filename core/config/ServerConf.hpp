#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../utils/params.hpp"
#include "../utils/utils.hpp"
#include "Routes.hpp"

namespace http
{

class ServerConf
{

private:

    typedef struct  sockaddr_in SA_IN;

    // Port server. Default 80
    in_port_t       _port;

    // Ip address of the server. Default 127.0.0.1
    std::string     _server_addr;

    // Server Name. Default locahost
    std::string     _server_name;

    // Default error pages
    std::map<int, std::string>  _err_pages;

    // Limit client body size
    unsigned int    _body_size;

    std::string                 _root;

    // Default file to answer if the request is a directory
    std::vector<std::string>     _index_file;

    SA_IN           _address;

    // Define a directory to the specific route. the route / is mandatory.
    std::vector<Routes>   _routes;

    //Authentication
    bool            _is_auth;

    //Message to Authenticate
    std::string     _auth_message;

    //Route of the password file Authentication
    std::string     _path_auth;

	bool			_default_server;

public:
    ServerConf():
        _port(DEFAULT_PORT), _server_addr(DEFAULT_SERVER_ADDR),
        _server_name(DEFAULT_SERVER_NAME), _body_size(DEFAULT_BODY_SIZE_MEGABYTES)
    {
		this->_err_pages[400] = DEFAULT_ERROR_PAGE_400;
        this->_err_pages[401] = DEFAULT_ERROR_PAGE_401;
        this->_err_pages[403] = DEFAULT_ERROR_PAGE_403;
        this->_err_pages[404] = DEFAULT_ERROR_PAGE_404;
        this->_err_pages[405] = DEFAULT_ERROR_PAGE_405;
        this->_err_pages[413] = DEFAULT_ERROR_PAGE_413;
        this->_err_pages[500] = DEFAULT_ERROR_PAGE_50X;
        this->_err_pages[505] = DEFAULT_ERROR_PAGE_505;
        this->_is_auth = false;
        this->_default_server = false;
        this->_auth_message = std::string("");
        this->_root = std::string("");
        this->_path_auth = std::string("");
        this->_address.sin_family = AF_INET;
        this->_address.sin_addr.s_addr = inet_addr(this->_server_addr.c_str());
        this->_address.sin_port = htons(this->_port);
    };
    virtual ~ServerConf(){};

    SA_IN       &getInfoAddress()
    {
        return (this->_address);
    };

    void        setPort(in_port_t newport)
    {
        //std::cout << "New server port -> " << newport << std::endl;
        this->_port = newport;
        this->_address.sin_port = htons(this->_port);
    };

    in_port_t   &getPort(void)
    {
        return (this->_port);
    };

    void        setServerAddr(std::string new_serveraddr)
    {
        //std::cout << "New server addr -> " << new_serveraddr << std::endl;
        this->_server_addr = new_serveraddr;
        this->_address.sin_addr.s_addr = inet_addr(this->_server_addr.c_str());
    };

    std::string &getServerAddr(void)
    {
        return (this->_server_addr);
    };

	void		setDefaultServer()
    {
		this->_default_server = true;
	}

	bool		isDefault(void)
    {
		return (this->_default_server);
	}

    void        setRootDir(std::string dir_path)
    {
        this->_root = dir_path;
    };

    std::string &getRootDir()
    {
        return (this->_root);
    };

    void        addIndexFile(std::string index_file)
    {
        this->_index_file.push_back(index_file);
    };

    std::vector<std::string> getIndexFile()
    {
        return (this->_index_file);
    };

    void        setServerName(std::string new_servername)
    {
        this->_server_name = new_servername;
    };

    std::string &getServerName(void)
    {
        return (this->_server_name);
    };

    void        setBodySize(unsigned int new_body_size)
    {
        this->_body_size = new_body_size;
    };

    unsigned int &getBodySize(void)
    {
        return (this->_body_size);
    };

    unsigned long long getBodySizeinBytes(void)
    {
        return (this->_body_size * 1000000);
    };

    void        setErrorPage(std::vector<int> codes, std::string file)
    {
        size_t  i = 0;

        while (i < codes.size())
            this->_err_pages[codes[i++]] = file;
    };

    std::string getErrorPage(int code)
    {
        return (this->_err_pages[code]);
    };

    std::map<int, std::string>  &getErrorPages()
    {
        return (this->_err_pages);
    };

    void        add_route(http::Routes new_route)
    {
        this->_routes.push_back(new_route);
    };

    void        set_routes(std::vector<http::Routes> all_route)
    {
        this->_routes = all_route;
    };

    std::vector<http::Routes>    getRoutes()
    {
        return (this->_routes);
    };

    void        allowAuth()
    {
        this->_is_auth = true;
    };

    bool        needAuth()
    {
        return (this->_is_auth);
    };

    void        setAuthMessage(std::string auth_mess)
    {
        this->allowAuth();
        this->_auth_message = trim2(auth_mess, "\"");
    };

    std::string &getAuthMessage()
    {
        return (this->_auth_message);
    };

    void        setPassAuthFile(std::string auth_file)
    {
        this->_path_auth = auth_file;
    };

    std::string &getPassAuthFile()
    {
        return (this->_path_auth);
    };

	std::vector<std::string> get_server_host_names()
    {
		std::vector<std::string> host_names;

		if (this->getPort() == 80 && this->getServerName() == "localhost")
			host_names.push_back("localhost");
		host_names.push_back(this->getServerName() + ":" + std::to_string(this->getPort()));
		host_names.push_back(this->getServerAddr() + ":" + std::to_string(this->getPort()));
		host_names.push_back("localhost:" + std::to_string(this->getPort()));
		return (host_names);
	}

    http::Routes    getRoutebyPath(std::string  &search_path)
    {
        std::vector<http::Routes>::iterator it = this->_routes.begin();
        std::vector<http::Routes>::iterator itend = this->_routes.end();
        http::Routes                        father_location;
        std::string                         ext = std::string("");
        std::string                         path;
        size_t                              len = 0;
        size_t                              max_len = 0;

        if (search_path.find(".") != std::string::npos)
            ext = search_path.substr(search_path.find(".") + 1);
        for (; it != itend; it++)
        {
            path = it->getVirtualLocation();
            len = path.length();
            if (path == search_path)
            {
                father_location = *it;
                max_len = len;
                break ;
            }
            else if (ext != "" && !it->isPrefix() && it->getExtension() == ext)
            {
                std::vector<http::Routes>::iterator itl = this->_routes.begin();
                std::vector<http::Routes>::iterator itlend = this->_routes.end();
                http::Routes                        father_location2;
                size_t                              len2 = 0;
                size_t                              max_len2 = 0;
                //std::cout << "sufix in" << std::endl;
                father_location = *it;
                max_len = len;
                for (; itl != itlend; itl++)
                {
                    path = itl->getVirtualLocation();
                    len2 = path.length();
                    //std::cout << "search path: " << search_path << " route path: " << path << std::endl;
                    if (search_path.compare(0, len2, path) == 0)
                    {
                        //std::cout << "IN" << std::endl;
                        if (!max_len2)
                        {
                            max_len2 = len2;
                            father_location2 = *itl;
                        }
                        else if (len2 > max_len2)
                        {
                            max_len2 = len2;
                            father_location2 = *itl;
                        }
                    }
                }
                //std::cout << "sufix out" << std::endl;
                if (max_len2)
                {
                    father_location2.setCgiExec(father_location.getCgiExec());
					father_location2.setExtension(father_location.getExtension());
                    return (father_location2);
                }
                else
                    return (father_location);
            }
            else if (search_path.compare(0, len, path) == 0)
            {
                if (!max_len)
                {
                    max_len = len;
                    father_location = *it;
                }
                else if (len > max_len)
                {
                    max_len = len;
                    father_location = *it;
                }
            }
        }
        if (max_len)
            return (father_location);
        else
            return (*itend);
    };
};



inline std::ostream &operator<<(std::ostream &out, http::ServerConf &server)
{
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator  itend;

    out << "\nServer Port: " << server.getPort() << std::endl;
    out << "Server Name: " << server.getServerName() << std::endl;
    out << "Server Address: " << server.getServerAddr() << std::endl;
    out << "Server BodySize: " << server.getBodySize() << std::endl;
    out << "Location Root: " << server.getRootDir() << std::endl;
    out << "Location indexFile: ";
    it = server.getIndexFile().begin();
    itend = server.getIndexFile().end();
    for (; it != itend; it++)
        out << *it << " ";
    out << std::endl;
    out << "Location authentication: " << ((server.needAuth()) ? "on" : "off") << std::endl;
    out << "Location AuthMessage: " << server.getAuthMessage() << std::endl;
    out << "Location Password Auth File: " << server.getPassAuthFile() << std::endl;
    out << "Error Pages: " << std::endl;
    std::map<int, std::string>::iterator itl = server.getErrorPages().begin();
    std::map<int, std::string>::iterator itlend = server.getErrorPages().end();
    for (; itl != itlend; itl++)
        out << "\t" << itl->first << " -> " << itl->second << std::endl;
    return (out);
};

} // namespace http

#endif

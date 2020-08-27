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

    SA_IN           _address;

    // Define a directory to the specific route. the route / is mandatory.
    std::vector<Routes>   _routes;

    //Authentication
    bool            _is_auth;

    //Message to Authenticate
    std::string     _auth_message;

    //Route of the password file Authentication
    std::string     _path_auth;

public:
    ServerConf():
        _port(DEFAULT_PORT), _server_addr(DEFAULT_SERVER_ADDR),
        _server_name(DEFAULT_SERVER_NAME), _body_size(DEFAULT_BODY_SIZE_MEGABYTES)
    {
        this->_err_pages[401] = DEFAULT_ERROR_PAGE_401;
        this->_err_pages[403] = DEFAULT_ERROR_PAGE_403;
        this->_err_pages[404] = DEFAULT_ERROR_PAGE_404;
        this->_err_pages[405] = DEFAULT_ERROR_PAGE_405;
        this->_err_pages[500] = DEFAULT_ERROR_PAGE_50X;
        this->_is_auth = false;
        this->_auth_message = std::string("");
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

    void        setServerName(std::string new_servername)
    {
        //std::cout << "New server name -> " << new_servername << std::endl;
        this->_server_name = new_servername;
    };

    std::string &getServerName(void)
    {
        return (this->_server_name);
    };

    void        setBodySize(unsigned int new_body_size)
    {
        //std::cout << "New server bodysize -> " << new_body_size << std::endl;
        this->_body_size = new_body_size;
    };

    unsigned int &getBodySize(void)
    {
        return (this->_body_size);
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

    http::Routes    &getRoutebyPath(std::string  &search_path)
    {
        std::vector<http::Routes>::iterator it = this->_routes.begin();
        std::vector<http::Routes>::iterator itend = this->_routes.end();
        std::vector<http::Routes>::iterator father_location;
        size_t  len = 0;
        size_t  max_len = 0;
        std::string path;
        for (; it != itend; it++)
        {
            path = it->getVirtualLocation();
            len = path.length();
            if (path == search_path)
            {
                father_location = it;
                max_len = len;
                break ;
            }
            else if (search_path.compare(0, len, path) == 0)
            {
                if (!max_len)
                {
                    max_len = len;
                    father_location = it;
                }
                else if (len > max_len)
                {
                    max_len = len;
                    father_location = it;
                }
            }
        }
        if (max_len)
            return (*father_location);
        else
            return (*itend);
    };
};

inline std::ostream &operator<<(std::ostream &out, http::ServerConf &server)
{
    out << "\nServer Port: " << server.getPort() << std::endl;
    out << "Server Name: " << server.getServerName() << std::endl;
    out << "Server Address: " << server.getServerAddr() << std::endl;
    out << "Server BodySize: " << server.getBodySize() << std::endl;
    out << "Location authentication: " << ((server.needAuth()) ? "on" : "off") << std::endl;
    out << "Location AuthMessage: " << server.getAuthMessage() << std::endl;
    out << "Location Password Auth File: " << server.getPassAuthFile() << std::endl;
    out << "Error Pages: " << std::endl;
    std::map<int, std::string>::iterator it = server.getErrorPages().begin();
    std::map<int, std::string>::iterator itend = server.getErrorPages().end();
    for (; it != itend; it++)
        out << "\t" << it->first << " -> " << it->second << std::endl;
    return (out);
};

} // namespace http

#endif

#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>
#include "../utils/params.hpp"
#include "Routes.hpp"

namespace http
{

class ServerConf
{

private:

    typedef struct  sockaddr_in SA_IN;

    typedef struct  s_error_pages
    {
        std::string _page404_path;
        std::string _page500_path;
    }               t_error_pages;

    // Port server. Default 80
    in_port_t       _port;

    // Ip address of the server. Default 127.0.0.1
    std::string     _server_addr;

    // Server Name. Default locahost
    std::string     _server_name;

    // Default error pages
    t_error_pages   _err_pages;

    // Limit client body size
    unsigned int    _body_size;

    SA_IN           _address;

    // Define a directory to the specific route. the route / is mandatory.
    std::vector<Routes>   _routes;

public:
    ServerConf():
        _port(DEFAULT_PORT), _server_addr(DEFAULT_SERVER_ADDR),
        _server_name(DEFAULT_SERVER_NAME), _body_size(DEFAULT_BODY_SIZE_MEGABYTES)
    {
        this->_err_pages._page404_path = DEFAULT_ERROR_PAGE_404;
        this->_err_pages._page500_path = DEFAULT_ERROR_PAGE_50X;
        this->_address.sin_family = AF_INET;
        this->_address.sin_addr.s_addr = inet_addr(this->_server_addr.c_str());
        this->_address.sin_port = htons(this->_port);
    };
    virtual ~ServerConf(){};

    void        setPort(in_port_t newport)
    {
        this->_port = newport;
        this->_address.sin_port = htons(this->_port);
    };

    in_port_t   &getPort(void)
    {
        return (this->_port);
    };

    void        setServerAddr(std::string new_serveraddr)
    {
        this->_server_addr = new_serveraddr;
        this->_address.sin_addr.s_addr = inet_addr(this->_server_addr.c_str());
    };

    std::string &getServerAddr(void)
    {
        return (this->_server_addr);
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

    void        add_route(http::Routes  new_route)
    {
        this->_routes.push_back(new_route);
    };

    http::Routes    &getRoutebyPath(std::string  &path)
    {
        std::vector<http::Routes>::iterator it = this->_routes.begin();
        std::vector<http::Routes>::iterator itend = this->_routes.end();

        for (; it != itend; it++)
            if (it->getVirtualLocation() == path)
                return (*it);
        //Excepcion y lanzar codigo 404
        return (*itend);
    };
};

inline std::ostream &operator<<(std::ostream &out, http::ServerConf &server)
{
    out << "\nSERVER INFO:\n";
    out << "Server Port: " << server.getPort() << std::endl;
    out << "Server Name: " << server.getServerName() << std::endl;
    out << "Server Address: " << server.getServerAddr() << std::endl;
    out << "Server BodySize: " << server.getBodySize() << std::endl;
    return (out);
};

} // namespace http

#endif

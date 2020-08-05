#ifndef SERVERCONF_HPP
#define SERVERCONF_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>
#include "../utils/params.hpp"

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

    typedef struct  s_httpMethods
    {

        bool    _GET;
        bool    _POST;
        bool    _HEAD;
        bool    _PUT;
        bool    _DELETE;
        bool    _CONNECT;
        bool    _OPTIONS;
        bool    _TRACE;
        bool    _PATCH;
    }               t_httpMethods;

    typedef struct  s_routes
    {
        //Virtual location of the directory
        std::string _location;

        // Path of a existing directory
        std::string _directory_path;

        //HTTP Methods in a specific location
        t_httpMethods   httpMethods;

        // Enable or disable directory listing. Default disable
        bool        _autoindex;

        // Default file to answer if the request is a directory
        std::string _index_file;

        /*
            CGI parameters must also be implemented here
        */

        // Make the route able to accept uploaded files. Default false
        bool        _uploads;

        // Configure where uploads should be saved
        std::string _upload_path;
    }               t_routes;

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

    //    Define a directory to the specific route. the route / is mandatory.
    std::vector<t_routes>   _routes;

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
};

} // namespace http

#endif

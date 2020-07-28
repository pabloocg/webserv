#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

# include <netinet/in.h>
# include <iostream>
# include <string>

class ServerConf
{

private:
    /* data */
protected:
    typedef struct sockaddr_in SA_IN;

    // Port server. Default 80
    in_port_t           port;

    // Ip address of the server. Default 127.0.0.1
    std::string         server_addr;

    // Server Name. Default locahost
    std::string         server_name;

    /*
        The first server for a host:port will be the default for
        this host:port (meaning it will answer to all request
        that doesn’t belong to an other server)
    */
    bool                ismain_server;

    // Default error pages
    struct error_pages {

        std::string     page404_path;
        std::string     page500_path;
    };

    // Limit client body size
    unsigned int        body_size;

    /*
        address.sin_family = AF_INET;
	    address.sin_addr.s_addr = inet_addr(server_addr);
	    address.sin_port = htons(port);
    */
    SA_IN               address;

    /*
        Define a directory or a file from where the file should be search
        For example:
        if url /kapouet is rooted to /tmp/www,
        url /kapouet/pouic/toto/pouet is/tmp/www/pouic/toto/pouet
    */
    struct  routes {

        //Virtual location of the directory
        std::string     _location;

        // Path of a existing directory
        std::string     _directory_path;

        //HTTP Methods in a specific location
        struct  httpMethods {

            bool        _GET;
            bool        _POST;
            bool        _HEAD;
            bool        _PUT;
            bool        _DELETE;
            bool        _CONNECT;
            bool        _OPTIONS;
            bool        _TRACE;
            bool        _PATCH;
        };

        // Enable or disable directory listing. Default disable
        bool            autoindex;

        // Default file to answer if the request is a directory
        std::string     index_file;

        /*
            CGI parameters must also be implemented here
        */

        // Make the route able to accept uploaded files. Default false
        bool        uploads;

        // Configure where uploads should be saved
        std::string upload_path;
    };
public:
    ServerConf();
    virtual ~ServerConf();

};

#endif

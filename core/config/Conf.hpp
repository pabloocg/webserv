#ifndef CONF_HPP
#define CONF_HPP

#include "../network/Server.hpp"
#include <vector>
#include <iostream>
#include <string>

#define DEFAULT_PORT 80
#define DEFAULT_SERVER_ADDR "127.0.0.1"
#define DEFAULT_ROOT_DIR "dir"
#define DEFAULT_FILE_INDEX "index.html"
#define DEFAULT_SERVER_NAME "localhost"
#define DEFAULT_BODY_SIZE_MEGABYTES 100
#define DEFAULT_BODY_SIZE_BYTES 1000000
#define DEFAULT_ERROR_PAGE_404 "404.html"
#define DEFAULT_ERROR_PAGE_50X "50x.html"
#define DEFAULT_ERROR_LOG "tmp/logs/error.log"
#define DEFAULT_ACCESS_LOG "tmp/logs/access.log"
#define DEFAULT_AUTOINDEX false
#define DEFAULT_ACCEPT_UPLOADS false
#define DEFAULT_UPLOADS_PATH "tmp/files/"
#define DEFAULT_HTTP_METHODS "GET POST HEAD PUT DELETE CONNECT OPTIONS TRACE PATCH"

namespace http
{

class Conf
{

private:

    Conf(void);

    const std::string           _filename;
    Logger                      _log;
    std::ifstream               _file;
    std::vector<http::Server>   _servers;

    bool        file_exists(void);
    void        read_file(void);
    

public:

    Conf(const std::string &conf_file);
    virtual ~Conf(void) {};

    std::vector<http::Server>   getServers(void);
};

} // namespace http

#endif

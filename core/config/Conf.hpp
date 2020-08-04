#ifndef CONF_HPP
#define CONF_HPP

#include "../network/Server.hpp"
#include <vector>
#include <iostream>
#include <string>

#define DEFAULT_FILE_CONF "../../example.conf"
#define DEFAULT_PORT 80
#define DEFAULT_SERVER_HOST "127.0.0.1"
#define DEFAULT_ROOT_DIR "dir"
#define DEFAULT_FILE_INDEX "index.html"
#define DEFAULT_SERVER_NAME "localhost"
#define DEFAULT_BODY_SIZE_MEGABYTES 100
#define DEFAULT_BODY_SIZE_BYTES 1000000
#define DEFAULT_ERROR_PAGE_404 "404.html"
#define DEFAULT_ERROR_PAGE_50X "50x.html"
#define DEFAULT_ERROR_LOG "logs/error.log"
#define DEFAULT_ACCESS_LOG "logs/access.log"

namespace http
{

class Conf
{

private:

    const std::string           _file;
    std::vector<http::Server>   _servers;

public:

    Conf(void);
    Conf(const std::string &conf_file);
    virtual ~Conf(void) {};

    std::vector<http::Server>   getServers(void);
};

} // namespace http

#endif

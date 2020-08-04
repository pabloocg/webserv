#include "Conf.hpp"

http::Conf::Conf(void):
            _file(DEFAULT_FILE_CONF)
{
}

http::Conf::Conf(const std::string &conf_file):
            _file(conf_file)
{
}

std::vector<http::Server>   http::Conf::getServers(void)
{
    return (_servers);
}

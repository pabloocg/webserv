#include "Conf.hpp"

http::Conf::Conf(const std::string &conf_file):
            _filename(conf_file),
            _log(ACCESS_LOG_PATH, ERROR_LOG_PATH)
{
    if (!file_exists())
        this->_log.makeLog(ERROR_LOG, "The entered configuration file doesn't exist"); exit(EXIT_FAILURE);
    read_file();
}

bool        http::Conf::file_exists(void)
{
    this->_file.open(this->_filename);
    return (this->_file.is_open());
}

// In process
void        http::Conf::read_file(void)
{
    std::string     strr;
    bool            nsHTTPfound = false;

    while (std::getline(this->_file, strr))
    {
        if (strr.find("http") != std::string::npos)
            nsHTTPfound = true;
    }
}

std::vector<http::Server>   http::Conf::getServers(void)
{
    return (this->_servers);
}

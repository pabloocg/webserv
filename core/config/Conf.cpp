#include "Conf.hpp"

http::Conf::Conf(const std::string &conf_file):
            _filename(conf_file),
            _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
    if (!file_exists())
    {
        this->_log.makeLog(ERROR_LOG, "The entered configuration file doesn't exist");
        exit(EXIT_FAILURE);
    }
    simple_parse();
    complex_parse();
}

bool        http::Conf::file_exists(void)
{
    this->_file.open(this->_filename);
    return (this->_file.is_open());
}

void        http::Conf::simple_parse(void)
{
    std::string     strr;
    size_t          pos;
    std::regex      comment("\\s*[#].*");

    while (std::getline(this->_file, strr))
    {
        if (!std::regex_match(strr, comment) and strr.length() > 0)
        {
            if ((pos = strr.find("#")) != std::string::npos)
                strr = strr.substr(0, pos);
            this->_simple_conf.push_back(http::trim(strr));
        }
    }
    this->_file.close();
}

void        http::Conf::complex_parse(void)
{
    for (std::vector<std::string>::iterator it = this->_simple_conf.begin(); it != this->_simple_conf.end(); it++)
    {
        std::cout << *it << std::endl;
    }
}

std::vector<http::ServerConf>   http::Conf::getServers(void)
{
    return (this->_servers);
}


const char* http::Conf::UnclosedBracket::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Unclosed Bracket");
    return ("File Configuration Exception: Unclosed Bracket");
}

const char* http::Conf::UnrecognizedParameter::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Unrecognized Parameter");
    return ("File Configuration Exception: Unrecognized Parameter");
}

const char* http::Conf::SemicolonMissing::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Semicolon Missing");
    return ("File Configuration Exception: Semicolon Missing");
}

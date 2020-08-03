#ifndef CONF_HPP
#define CONF_HPP

#include "ServerConf.hpp"
#include "../utils/utils.hpp"
#include "Logger.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <regex>

namespace http
{

class Conf
{

private:

    Conf(void);

    const std::string                   _filename;
    http::Logger                        _log;
    std::ifstream                       _file;
    std::vector<http::ServerConf>       _servers;
    std::map<std::string, bool>         _mandatory_conf;
    std::vector<std::string>            _simple_conf;


    bool        file_exists(void);
    std::string        simple_parse(void);
    void        complex_parse(std::string s);
    

public:

    Conf(const std::string &conf_file);
    virtual ~Conf(void) {};

    std::vector<http::ServerConf>   getServers(void);

    class UnclosedBracket: public std::exception {
        virtual const char* what() const throw();
    };

    class UnrecognizedParameter: public std::exception {
        virtual const char* what() const throw();
    };

    class SemicolonMissing: public std::exception {
        virtual const char* what() const throw();
    };
};

} // namespace http

#endif

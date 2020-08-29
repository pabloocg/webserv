#ifndef CONF_HPP
#define CONF_HPP

#include "ServerConf.hpp"
#include "../utils/utils.hpp"
#include "Routes.hpp"
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

    std::string                         _filename;
    http::Logger                        _log;
    std::ifstream                       _file;
    std::vector<http::ServerConf>       _servers;
    std::map<std::string, bool>         _mandatory_conf;
    std::vector<std::string>            _simple_conf;
	std::map<std::string, std::string>	_mime_types;
    static const std::string            _http_methods[9];


    bool            file_exists(void);
    std::string     simple_parse(void);
    void            complex_parse(std::string s);
    void            parse_server_conf(std::string s);
	void            parse_types(std::string s);
    http::Routes    save_location(std::string s, std::string opt, std::string vpath);
    http::Routes    check_inheritance(http::Routes &locat, std::vector<http::Routes> all_routes, int i_server);

    class UnclosedBracket: public std::exception {
        virtual const char* what() const throw();
    };

    class UnrecognizedParameter: public std::exception {
        virtual const char* what() const throw();
    };

    class SemicolonMissing: public std::exception {
        virtual const char* what() const throw();
    };

    class ConfFileNotExists: public std::exception {
        virtual const char* what() const throw();
    };

    class ErrorInConf: public std::exception {
        virtual const char* what() const throw();
    };

public:

    Conf(const std::string &conf_file);
    virtual ~Conf(void) {};

    std::vector<http::ServerConf>       getServers(void);
	std::map<std::string, std::string>  get_mime_types(void);

};

} // namespace http

#endif

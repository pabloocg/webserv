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
    complex_parse(simple_parse());
}

bool        http::Conf::file_exists(void)
{
    this->_file.open(this->_filename);
    return (this->_file.is_open());
}

std::string        http::Conf::simple_parse(void)
{
    std::string     strr;
    size_t          pos;
    std::regex      comment("\\s*[#].*");
    std::stringstream   buf;

    while (std::getline(this->_file, strr))
    {
        if (!std::regex_match(strr, comment) and strr.length() > 0)
        {
            if ((pos = strr.find("#")) != std::string::npos)
                strr = strr.substr(0, pos);
            buf << http::trim(strr);
        }
    }
    this->_file.close();
    return (buf.str());
}

void        http::Conf::complex_parse(std::string s)
{
    size_t      i = 0;
    std::string string_p;

    if (std::count(s.begin(), s.end(), '{') != std::count(s.begin(), s.end(), '}'))
        throw Conf::UnclosedBracket();
    if (!s.compare(0, 4, "http"))
        std::cout << "namespace http found" << std::endl;
    else
        throw Conf::UnrecognizedParameter();
    i = 4;
    while (std::isspace(s[i]))
        i++;
    if (!(s[i] == '{'))
        throw Conf::UnrecognizedParameter();
    string_p = s.substr(i + 1, s.find_last_of("}") - (i + 1));
    /*i = -1;
    while (string_p[++i])
    {
        if (!(s[i] == '{'))


    }
    */
    std::cout << string_p << std::endl;
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

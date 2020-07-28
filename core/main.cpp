#include "network/Server.hpp"
//#include "config/Conf.hpp"

//#define DEFAULT_FILE_CONF "example.conf"

int main(/*int argc, char *argv[]*/)
{
    /*
    std::string file_conf;

    if (argc == 1)
        file_conf = std::string(DEFAULT_FILE_CONF);
    else if (argc == 2)
        file_conf = std::string(argv[1]);
    else
        std::cerr << "Usage: ./webserv <path_file_conf>" << std::endl; return (0);
    http::Conf  config(file_conf);
    */
    http::Server serv;
    serv.start();
    while (1)
    {
        serv.wait_for_connection();
    }
    return 0;
}

#include "network/Server.hpp"
#include "utils/Request.hpp"

//#define CONFIG_TEST

#ifdef CONFIG_TEST

# include "config/Conf.hpp"
# define DEFAULT_FILE_CONF "example.conf"
# include <vector>

#endif

int main(int argc, char *argv[])
{
    #ifdef CONFIG_TEST

        std::string file_conf;

        if (argc == 1)
            file_conf = std::string(DEFAULT_FILE_CONF);
        else if (argc == 2)
            file_conf = std::string(argv[1]);
        else
        {
            std::cerr << "Usage: ./webserv <path_file_conf>" << std::endl;
            return (0);
        }
        try
        {
            http::Conf  config(file_conf);
            std::vector<http::ServerConf>   servers(config.getServers());
            for (std::vector<http::ServerConf>::iterator it = servers.begin(); it != servers.end(); it++)
                std::cout << *it << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

    #endif

    #ifndef CONFIG_TEST
        (void)argc;(void)argv;
        // Cuando la clase de coger la configuracion esté terminada
        //http::Server serv(config.getServers());
        http::Server serv;
        serv.start();
        while (1)
        {
            serv.wait_for_connection();
        }

    #endif
    return 0;
}

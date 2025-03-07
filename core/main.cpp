#include "config/Conf.hpp"
#include "network/ServerC.hpp"

#define DEFAULT_FILE_CONF "tests/example.conf"

void			handle_signal(int signal)
{
	if (signal == SIGINT)
    {
        std::cout << "\nStopping webserv..." << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[], char *env[])
{
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
    signal(SIGINT, handle_signal);
    signal(SIGPIPE, SIG_IGN);
    try
    {
        http::Conf                      config(file_conf);
        http::ServerC serv(config.getServers(), config.get_mime_types(), env);

        serv.start();
        while (1)
            serv.wait_for_connection();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

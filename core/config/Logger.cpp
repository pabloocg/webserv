#include "Logger.hpp"

http::Logger::Logger(const std::string accs, const std::string err):
            _access(accs),
            _error(err)
{
}

http::Logger::~Logger()
{
}

void        http::Logger::logToAccess(std::string login_message) const
{
    std::ofstream           new_file;

    new_file.open(this->_access, std::ios::app);
    new_file << login_message << std::endl;
    new_file.close();
}

void        http::Logger::logToError(std::string login_message) const
{
    std::ofstream           new_file;

    new_file.open(this->_error, std::ios::app);
    new_file << login_message << std::endl;
    new_file.close();
}

std::string http::Logger::makeLogEntry(std::string message) const
{
    std::time_t         t;
    std::tm*            now;
    std::stringstream   buffer;

    t = std::time(0);
    now = std::localtime(&t);
    buffer << std::setfill('0') << (now->tm_year + 1900) << "-" <<
    std::setw(2) << (now->tm_mon + 1) << "-" << std::setw(2) << now->tm_mday
    << " " << std::setw(2) << now->tm_hour << ":" << std::setw(2)
    << now->tm_min << ":" << std::setw(2) << now->tm_sec << "\tMessage: " << message;
    return (buffer.str());
}

void        http::Logger::makeLog(std::string const & dest, std::string const & message) const
{
    static void    (http::Logger::*point_logs[])(std::string login_message) const = {
            &http::Logger::logToAccess,
            &http::Logger::logToError
    };
    static const std::string log_type[] = {
        "access.log",
        "error.log"
    };

    for (int i = 0; i < 2; i++)
    {
        if (dest == log_type[i])
        {
            (this->*point_logs[i])(makeLogEntry(message));
            return ;
        }
    }
}

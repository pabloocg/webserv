#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

namespace http
{

class Logger
{
    private:

        Logger();

        const std::string _access;
        const std::string _error;

        void        logToAccess(std::string login_message) const;
        void        logToError(std::string login_message) const;
        std::string makeLogEntry(std::string message) const;
    public:

        Logger(const std::string accs, const std::string err);
        virtual ~Logger();

        void        makeLog(std::string const & dest, std::string const & message) const;
};

}

#endif

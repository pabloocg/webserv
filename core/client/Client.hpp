#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <vector>
#include "../utils/utils.hpp"
#include "../utils/params.hpp"

namespace http
{

class Client
{
private:
    int     _fd;
	struct timeval	_time_last_activity;

    /*  For Pending Send Response*/
    char    *_message_send;
    bool    _send_message_free;
    ssize_t     _size_send;
    ssize_t     _sended;
    ssize_t     _left;
    bool    _is_sending;

    /*  For Pending Read Request*/
    std::string _message;
    std::string _headers;
    std::string _host_header;
    bool        _read_message_free;
	char		*_dechunked_body;
	char		*_last_read;
    size_t         _bodyLength;
	int			_offset;
	int			_size_char;
	int			_size_nl;
	int			_valread;
	int			_state;
	int			_dechunked_capacity;
	int			_dechunked_size;
    bool        _isChunked;
    bool        _isLength;
    bool        _headers_read;
    int         _code;
    bool        _is_reading;
	bool		_first_time;

public:
    Client();
    Client(int &fd);
    ~Client();

    http::Client    &operator=(const http::Client &other);

    int     &getFd();
    void    setFd(int &new_socket);
	void	set_last_activity(void);
	struct timeval get_time_sleeping(struct timeval now);

    /*  For Pending Send Response*/
    bool    isSending(void);
    void    setupSend(char *message, ssize_t size, ssize_t sended, ssize_t left);
    void    setSending(bool t);
    char    *getSendMessage(void);
    ssize_t     getSendSize(void);
    ssize_t     getSendLeft(void);
    ssize_t     getSended(void);
    void    setSended(ssize_t sended);
    void    setSendLeft(ssize_t sended);
    void    reset_send(void);
    void    freeSendMessage(void);

    /*  For Pending Read Request*/
	void		handle_chunked(void);
    bool        isReading(void);
    void        setReading(bool t);
    void        appendReadMessage(std::string message);
    void        setReadMessage(std::string message);
    bool        read_valid_format(char *last_read, int valread);
    std::string getHeaders();
    std::string getMessage();
    int         getCodeStatus();
    std::string getHostHeader();
	char		*get_dechunked_body();
    void        reset_read(void);
    class		ServerError: public std::exception
	{
		private:
			std::string		_error;

		public:
			ServerError(void);
			ServerError(std::string function, std::string error);
			virtual				~ServerError(void) throw() {};
			virtual const char	*what(void) const throw();
	};
};
} // namespace http

#endif

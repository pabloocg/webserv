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
    int     _size_send;
    int     _sended;
    int     _left;
    bool    _is_sending;

    /*  For Pending Read Request*/
    std::string _message;
    std::string _headers;
    std::string _host_header;
	char		*_char_message;
	char		*_dechunked_body;
	char		*_last_read;
    int         _bodyLength;
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
    virtual ~Client();

    http::Client    &operator=(const http::Client &other);

    int     &getFd();
    void    setFd(int &new_socket);
	void	set_last_activity(void);
	struct timeval get_time_sleeping(struct timeval now);

    /*  For Pending Send Response*/
    bool    isSending(void);
    void    setupSend(char *message, int size, int sended, int left);
    void    setSending(bool t);
    char    *getSendMessage(void);
    int     getSendSize(void);
    int     getSendLeft(void);
    int     getSended(void);
    void    setSended(int sended);
    void    setSendLeft(int sended);
    void    reset_send(void);

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
};
} // namespace http

#endif

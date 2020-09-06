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
    int         _bodyLength;
    bool        _isChunked;
    bool        _isLength;
    bool        _headers_read;
    bool        _badRequest;
    bool        _is_reading;

public:
    Client();
    Client(int &fd);
    virtual ~Client();

    int     &getFd();
    void    setFd(int &new_socket);

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
    bool        isReading(void);
    void        setReading(bool t);
    void        appendReadMessage(std::string message);
    void        setReadMessage(std::string message);
    bool        read_valid_format(void);
    std::string getHeaders();
    std::string getMessage();
    bool        getBadRequest();
    std::string getHostHeader();
    void        reset_read(void);
};
} // namespace http

#endif

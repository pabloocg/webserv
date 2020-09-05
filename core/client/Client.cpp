#include "Client.hpp"

http::Client::Client(): _fd(0)
{
    this->reset_read();
}

http::Client::Client(int &fd): _fd(fd)
{
    this->reset_read();
}

http::Client::~Client()
{
    this->_fd = 0;
}

void    http::Client::reset_read(void)
{
    this->_is_sending = false;
    this->_is_reading = false;
    this->_message = "";
	this->_host_header = "NULL";
	this->_headers_read = false;
	this->_isChunked = false;
	this->_isLength = false;
	this->_badRequest = false;
	this->_bodyLength = 0;
}

int     &http::Client::getFd()
{
    return (this->_fd);
}

void    http::Client::setFd(int &new_socket)
{
    this->_fd = new_socket;
}

bool    http::Client::isSending(void)
{
    return (this->_is_sending);
}

void    http::Client::setSending(bool t)
{
    this->_is_sending = t;
}

bool    http::Client::isReading(void)
{
    return (this->_is_reading);
}

void    http::Client::setReading(bool t)
{
    this->_is_reading = t;
}

void    http::Client::appendReadMessage(std::string message)
{
    this->_message.append(message);
}

void    http::Client::setReadMessage(std::string message)
{
    this->_message = std::string(message);
}

std::string http::Client::getHeaders()
{
    return (this->_headers);
}

std::string http::Client::getMessage()
{
    return (this->_message);
}

bool        http::Client::getBadRequest()
{
    return (this->_badRequest);
}

std::string http::Client::getHostHeader()
{
    return (this->_host_header);
}

bool http::Client::read_valid_format(void)
{
	size_t                      end_headers = 0;
    size_t                      message_len;
    std::vector<std::string>	splitted_headers;

#ifdef  DEBUG_MODE

	std::cout << "lleva leido " << this->_message.length() << std::endl;
    //std::cout << "Message:\n" << this->_message << std::endl;

#endif

	if (!this->_headers_read)
    {
        end_headers = this->_message.find("\r\n\r\n");
		if (end_headers != std::string::npos)
		{
			this->_headers_read = true;
			splitted_headers = http::split(this->_message.substr(0, end_headers), '\n');
			this->_headers = this->_message.substr(0, end_headers + 4);
			this->_message = this->_message.substr(end_headers + 4);
			for (size_t i = 0; i < splitted_headers.size(); i++)
			{
				if (splitted_headers[i].find("Content-Length:") != std::string::npos)
					this->_bodyLength = std::atoi(splitted_headers[i].substr(16, splitted_headers[i].length() - 16).c_str());
				else if (splitted_headers[i].find("Transfer-Encoding: chunked") != std::string::npos)
					this->_isChunked = true;
				else if (splitted_headers[i].find("Host:") != std::string::npos)
				{
					if (this->_host_header == "NULL")
						this->_host_header = splitted_headers[i].substr(6, splitted_headers[i].length() - 7);
					else
						this->_badRequest = true;
				}
			}
			if (this->_badRequest || this->_host_header == "NULL")
            {
				this->_badRequest = true;
				return (true);
			}
		}
	}
	if (this->_headers_read)
    {
		message_len = this->_message.length();
		if (this->_isChunked && message_len > 0 && this->_message[message_len - 5] == '0'
                            && this->_message[message_len - 4] == '\r'
                            && this->_message[message_len - 3] == '\n'
                            && this->_message[message_len - 2] == '\r'
                            && this->_message[message_len - 1] == '\n')
			return (true);
		if (!this->_isChunked && (this->_bodyLength == (int)this->_message.length()
                                    || this->_bodyLength == (int)this->_message.length() - 2))
			return (true);
	}
	return (false);
}

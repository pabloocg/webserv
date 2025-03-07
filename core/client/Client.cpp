#include "Client.hpp"

#define NEED_SIZE 1
#define NEED_R_1 2
#define NEED_N_1 3
#define GO_READ 4
#define NEED_R_2 5
#define NEED_N_2 6
#define WAIT_END 7

http::Client::Client() : _fd(0)
{
	this->_dechunked_body = NULL;
	this->_message_send = NULL;
	this->_send_message_free = false;
	this->reset_read();
	this->reset_send();
	set_last_activity();
}

http::Client::Client(int &fd) : _fd(fd)
{
	this->_dechunked_body = NULL;
	this->_message_send = NULL;
	this->reset_read();
	this->reset_send();
	set_last_activity();
}

http::Client::~Client()
{
	this->_fd = 0;
	if (this->_dechunked_body != NULL)
	{
		free(this->_dechunked_body);
		this->_dechunked_body = NULL;
	}
}

http::Client &http::Client::operator=(const http::Client &other)
{
	this->_fd = other._fd;
	this->_message_send = other._message_send;
	this->_size_send = other._size_send;
	this->_sended = other._sended;
	this->_left = other._left;
	this->_is_sending = other._is_sending;
	this->_message = other._message;
	this->_headers = other._headers;
	this->_code = other._code;
	this->_bodyLength = other._bodyLength;
	this->_headers_read = other._headers_read;
	this->_host_header = other._host_header;
	this->_is_reading = other._is_reading;
	this->_isChunked = other._isChunked;
	this->_isLength = other._isLength;
	return (*this);
}

void http::Client::reset_send(void)
{
	this->_is_sending = false;
	this->_send_message_free = false;
	this->_message_send = NULL;
	this->_size_send = 0;
	this->_sended = 0;
	this->_left = 0;
}

void http::Client::reset_read(void)
{
	this->_is_reading = false;
	this->_read_message_free = false;
	this->_message = "";
	this->_host_header = "NULL";
	this->_headers_read = false;
	this->_isChunked = false;
	this->_isLength = false;
	this->_code = 0;
	this->_bodyLength = 0;
	this->_offset = 0;
	this->_size_nl = 0;
	this->_state = NEED_SIZE;
	this->_dechunked_size = 0;
	this->_dechunked_capacity = 0;
	this->_dechunked_body = NULL;
	this->_first_time = true;
	
}

int &http::Client::getFd()
{
	return (this->_fd);
}

void http::Client::setFd(int &new_socket)
{
	this->_fd = new_socket;
}

void http::Client::freeSendMessage()
{
	if (this->_send_message_free && this->_message_send != NULL)
		free(this->_message_send);
	this->_send_message_free = false;
	this->_message_send = NULL;
}

void http::Client::setupSend(char *message, ssize_t size, ssize_t sended, ssize_t left)
{
	this->_message_send = message;
	this->_send_message_free = true;
	this->_size_send = size;
	this->_sended = sended;
	this->_left = left;
}

bool http::Client::isSending(void)
{
	return (this->_is_sending);
}

void http::Client::setSending(bool t)
{
	this->_is_sending = t;
}

char *http::Client::getSendMessage(void)
{
	return (this->_message_send);
}

ssize_t http::Client::getSendSize(void)
{
	return (this->_size_send);
}

struct timeval http::Client::get_time_sleeping(struct timeval now)
{
	struct timeval time_sleeping;

	time_sleeping.tv_usec = now.tv_usec - this->_time_last_activity.tv_usec;
	if (time_sleeping.tv_usec < 0)
	{
		time_sleeping.tv_usec += 1000000;
		time_sleeping.tv_sec = now.tv_sec - this->_time_last_activity.tv_sec - 1;
	}
	else
	{
		time_sleeping.tv_sec = now.tv_sec - this->_time_last_activity.tv_sec ;
	}
	return(time_sleeping);
}

ssize_t http::Client::getSended(void)
{
	return (this->_sended);
}

ssize_t http::Client::getSendLeft(void)
{
	return (this->_left);
}

void http::Client::setSended(ssize_t sended)
{
	this->_sended = sended;
}

void http::Client::setSendLeft(ssize_t left)
{
	this->_left = left;
}

bool http::Client::isReading(void)
{
	return (this->_is_reading);
}

void http::Client::setReading(bool t)
{
	this->_is_reading = t;
}

void http::Client::appendReadMessage(std::string message)
{
	this->_message.append(message);
}

void http::Client::setReadMessage(std::string message)
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


void	http::Client::set_last_activity(void)
{
	if (gettimeofday(&this->_time_last_activity, NULL) != 0)
	{
		perror("gettimeofday");
	}
}

int http::Client::getCodeStatus()
{
	return (this->_code);
}

std::string http::Client::getHostHeader()
{
	return (this->_host_header);
}

bool http::Client::read_valid_format(char *last_read, int valread)
{
	size_t end_headers = 0;
	//this->_bodyLength = 0;
	size_t message_len;
	std::vector<std::string> splitted_headers;
	this->_last_read = last_read;
	this->_valread = valread;
	int		method = 0;

#ifdef DEBUG_MODE

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
			if (!method && splitted_headers[0].find("GET ") != std::string::npos)
				method = GET;
			else if (!method && splitted_headers[0].find("HEAD ") != std::string::npos)
				method = HEAD;
			else if (!method && splitted_headers[0].find("OPTIONS ") != std::string::npos)
				method = OPTIONS;
			else if (!method && splitted_headers[0].find("POST ") != std::string::npos)
				method = POST;
			else if (!method && splitted_headers[0].find("PUT ") != std::string::npos)
				method = PUT;
			else if (!method && splitted_headers[0].find("DELETE ") != std::string::npos)
				method = DELETE;
			else
				method = NOT_IMPLEMENTED;
			for (size_t i = 1; i < splitted_headers.size(); i++)
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
						this->_code = 400;
				}
			}
			if (this->_host_header == "NULL")
				this->_code = 400;
			if (method == NOT_IMPLEMENTED)
			{
				this->_code = 501;
				return (true);
			}
			if (method > 1 && method < 4 && !this->_bodyLength && !this->_isChunked)
			{
				this->_code = 411;
				return (true);
			}
		}
	}
#ifdef DEBUG_MODE
	std::cout << "HEADERS FOUND bodyL -> " << this->_bodyLength<< "MessageL-> "<< this->_message.length()  << std::endl;
#endif
	if (this->_headers_read)
	{
		if (this->_isChunked)
		{
			message_len = this->_message.length();
			handle_chunked();
			if (message_len > 4 && this->_message[message_len - 5] == '0'
								&& this->_message[message_len - 4] == '\r'
								&& this->_message[message_len - 3] == '\n'
								&& this->_message[message_len - 2] == '\r'
								&& this->_message[message_len - 1] == '\n')
			{
				if (this->_dechunked_size > 0)
					this->_dechunked_body[this->_dechunked_size] = '\0';
#ifdef DEBUG_MODE
				std::cout << "dechunked size:" << this->_dechunked_size << std::endl;
#endif
				return (true);
			}
		}
		if (!this->_isChunked && (this->_bodyLength == this->_message.length() || this->_bodyLength == this->_message.length() - 2))
			return (true);
	}
	return (false);
}

void http::Client::handle_chunked(void)
{
	int i = this->_offset - 1;
	const char *ptr = this->_message.c_str();
	this->_size_char = this->_message.length();
	while (++i < this->_size_char)
	{
		if (this->_state == NEED_SIZE)
		{
			if (ptr[i] >= '0' && ptr[i] <= '9')
				this->_size_nl = (this->_size_nl << 4) + (ptr[i] - '0');
			else if (ptr[i] >= 'a' && ptr[i] <= 'f')
				this->_size_nl = (this->_size_nl << 4) + (ptr[i] - 'a' + 10);
			else if (ptr[i] >= 'A' && ptr[i] <= 'F')
				this->_size_nl = (this->_size_nl << 4) + (ptr[i] - 'A' + 10);
			else if (this->_size_nl > 0)
				this->_state = NEED_N_1;
			else
				this->_state = WAIT_END;
		}
		else if (this->_state == NEED_N_1 && ptr[i] == '\n')
			this->_state = GO_READ;
		else if (this->_state == GO_READ)
		{
			int to_read = this->_size_nl;
			if (this->_size_char - i < to_read)
				to_read = this->_size_char - i;
			this->_size_nl -= to_read;
			if (this->_dechunked_capacity < this->_dechunked_size + to_read + 1)
			{
				char *tmp;
				if (!(tmp = (char *)malloc(this->_dechunked_capacity + to_read + 10000000)))
					throw ServerError("malloc", "allocating memory");
				if (this->_dechunked_size > 0)
				{
					memcpy(tmp, this->_dechunked_body, this->_dechunked_size);
					free(this->_dechunked_body);
				}
				this->_dechunked_body = tmp;
				this->_dechunked_capacity += to_read + 10000000;
			}
			memcpy(this->_dechunked_body + this->_dechunked_size, ptr + i, to_read);
			this->_dechunked_size += to_read;
			this->_dechunked_body[this->_dechunked_size] = '\0';
			if (this->_size_nl == 0)
				this->_state = NEED_N_2;
		}
		else if (this->_state == NEED_N_2 && ptr[i] == '\n')
			this->_state = NEED_SIZE;
		else if (this->_state == WAIT_END)
			;
	}
	this->_offset = i;
}

char *http::Client::get_dechunked_body(void)
{
	return (this->_dechunked_body);
}

http::Client::ServerError::ServerError(void)
{
	this->_error = "Undefined Server Exception";
}

http::Client::ServerError::ServerError(std::string function, std::string error)
{
	this->_error = function + ": " + error;
}

const char *http::Client::ServerError::what(void) const throw()
{
	return (this->_error.c_str());
}

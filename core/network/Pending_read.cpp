#include "Pending_read.hpp"


http::Pending_read::Pending_read(void){
	message = "";
	host_header = "NULL";
	headers_read = false;
	isChunked = false;
	isLength = false;
	badRequest = false;
	bodyLength = 0;
	

}

void http::Pending_read::operator=(http::Pending_read const &other){
	this->message = other.message;
	this->isChunked = other.isChunked;
	this->isLength = other.isLength;
	this->headers_read = other.headers_read;
	this->bodyLength = other.bodyLength;
	this->badRequest = other.badRequest;
}

bool http::Pending_read::is_valid_format(void){
	int i = 0;

	if (this->headers_read == false){
		std::vector<std::string>	splitted_headers;
		if (message.find("\r\n\r\n") != std::string::npos)
		{
			i = message.find("\r\n\r\n");
			this->headers_read = true;
			splitted_headers = http::split(message.substr(0, i), '\n');
			this->headers = this->message.substr(0, i + 4);
			this->message = this->message.substr(i + 4);
			for (int i = 0; i < (int)splitted_headers.size(); i++)
			{
				if (splitted_headers[i].find("Content-Length:") != std::string::npos)
					bodyLength = std::atoi(splitted_headers[i].substr(16, splitted_headers[i].length() - 16).c_str());
				else if (splitted_headers[i].find("Transfer-Encoding: chunked") != std::string::npos)
					isChunked = true;
				else if (splitted_headers[i].find("Host:") != std::string::npos)
				{
					if (host_header == "NULL")
						host_header = splitted_headers[i].substr(6, splitted_headers[i].length() - 7);
					else
						badRequest = true;
				}
			}
			if (badRequest || host_header == "NULL"){
				badRequest = true;
				return (true);
			}
		}
	}
	if (headers_read == true){
		int message_len = message.length();
		if (isChunked && message[message_len - 5] == '0'  && message[message_len - 4] == '\r'  && message[message_len - 3] == '\n'  && message[message_len - 2] == '\r'  && message[message_len - 1] == '\n')
			return (true);
		if (!isChunked && (bodyLength == (int)message.length() ||
			bodyLength == (int)message.length() - 2))
			return (true);
	}
	return (false);
}

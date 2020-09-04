#include "ServerC.hpp"

void	http::ServerC::read_request(char *buf, int &sd)
{
    int         size;
    int         bytes_send;
    char        *message;
    std::string s_buffer;

    if (this->_pending_reads.find(sd) != this->_pending_reads.end())
        this->_pending_reads[sd].message.append(std::string(buf));
    else{
		//this->_pending_reads[sd] = *(new Pending_read());
        this->_pending_reads[sd].message = std::string(buf);
	}
    if (this->_pending_reads[sd].is_valid_format())
    {
        s_buffer = this->_pending_reads[sd].headers + this->_pending_reads[sd].message;
		this->_bad_request = this->_pending_reads[sd].badRequest;
		this->_host_header = this->_pending_reads[sd].host_header;
		//delete(&(this->_pending_reads[sd]));
        this->_pending_reads.erase(sd);
        http::Request req(s_buffer, get_server(), this->_bad_request, this->_env);
        if (!(message = req.build_response(&size, _mime_types)))
            throw ServerError("request", "failed for some reason");
        if ((bytes_send = send(sd, message, (size_t)size, 0)) < 0)
            throw ServerError("send", "failed for some reason");
        else if (bytes_send < size)
        {
            http::Pending_send sended(message, size, bytes_send, size - bytes_send);
            std::pair<int, http::Pending_send> pair(sd, sended);
            _pending_messages.insert(pair);
            FD_SET(sd, &_master_write);
        }
        else
            free(message);

#ifdef 		DEBUG_MODE

        std::cout << "Sended " << bytes_send << " bytes to " << sd << ", " << size - bytes_send << " left" << std::endl;

#endif

    }
}

bool http::ServerC::valid_req_format(std::string buffer)
{
	std::vector<std::string>	splitted_req;
	long long					buf_len = buffer.length();
	std::string 	            headers;
	std::string 	            chunked_str;
	int				            body_size = 0;
	int				            end_headers;
	bool			            chunked = false;
	size_t			            i;

#ifdef 		DEBUG_MODE

	std::cout << "lleva leido " << buf_len << std::endl;

#endif

	if ((i = buffer.find("\r\n\r\n")) != std::string::npos)
	{
		splitted_req = http::split(buffer.substr(0, i), '\n');
		end_headers = i + 4;
	}
	this->_host_header = "NULL";
	this->_bad_request = false;
	if (splitted_req.size() < 2)
		return (false);
	for (int i = 0; i < (int)splitted_req.size(); i++)
	{
		if (splitted_req[i].find("Content-Length:") != std::string::npos)
			body_size = std::atoi(splitted_req[i].substr(16, splitted_req[i].length() - 16).c_str());
		else if (splitted_req[i].find("Transfer-Encoding: chunked") != std::string::npos)
			chunked = true;
		else if (splitted_req[i].find("Host:") != std::string::npos)
		{
			if (this->_host_header == "NULL")
				this->_host_header = splitted_req[i].substr(6, splitted_req[i].length() - 7);
			else
				this->_bad_request = true;
		}
	}
	if (chunked == true && buffer[buf_len - 5] == '0' && buffer[buf_len - 2] == '\r')
		return (true);
	if (chunked == false && (end_headers + body_size == (int)buffer.length() ||
							end_headers + body_size == (int)buffer.length() - 2))
	{
		if (this->_host_header == "NULL")
			this->_bad_request = true;
		return (true);
	}
	return (false);
}

#include "ServerC.hpp"

void	http::ServerC::read_request(char *buf, std::vector<http::Client>::iterator &client)
{
    int         size;
    int         bytes_send;
    char        *message;
    std::string s_buffer;

	if (client->isReading())
	{
		client->appendReadMessage(std::string(buf));
	}
	else
	{
		client->setReadMessage(std::string(buf));
		client->setReading(true);
	}
    if (client->read_valid_format())
    {
		s_buffer = client->getHeaders() + client->getMessage();
		this->_bad_request = client->getBadRequest();
		this->_host_header = client->getHostHeader();
		client->setReading(false);
		client->reset_read();
        http::Request req(s_buffer, get_server(), this->_bad_request, this->_env);
        if (!(message = req.build_response(&size, _mime_types)))
            throw ServerError("request", "failed for some reason");
        if ((bytes_send = send(client->getFd(), message, (size_t)size, 0)) < 0)
            throw ServerError("send", "failed for some reason");
        else if (bytes_send < size)
        {
            http::Pending_send sended(message, size, bytes_send, size - bytes_send);
            std::pair<int, http::Pending_send> pair(client->getFd(), sended);
            _pending_messages.insert(pair);
            FD_SET(client->getFd(), &_master_write);
        }
        else
            free(message);

#ifdef 		DEBUG_MODE

        std::cout << "Sended " << bytes_send << " bytes to " << client->getFd() << ", " << size - bytes_send << " left" << std::endl;

#endif

    }
}

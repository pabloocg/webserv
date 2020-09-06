#include "ServerC.hpp"

void	http::ServerC::read_request(char *buf, std::vector<http::Client>::iterator &client)
{
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
		client->setReading(false);
		s_buffer = client->getHeaders() + client->getMessage();
		this->_bad_request = client->getBadRequest();
		this->_host_header = client->getHostHeader();
		client->reset_read();
        this->send_response(s_buffer, client);
    }
}

void	http::ServerC::send_response(std::string &request, std::vector<http::Client>::iterator &client)
{
    ssize_t     size;
    ssize_t     bytes_send;
    char        *message;

    http::Request req(request, get_server(), this->_bad_request, this->_env);
    if (!(message = req.build_response(&size, _mime_types)))
        throw ServerError("request", "failed for some reason");
    if ((bytes_send = send(client->getFd(), message, (size_t)size, 0)) <= 0)
    {
        //throw ServerError("send", "failed for some reason");
        this->remove_client(client);
        return ;
    }
    else if (bytes_send < size)
    {
        client->setSending(true);
        client->setupSend(message, size, bytes_send, size - bytes_send);
        FD_SET(client->getFd(), &_master_write);
    }
    else
        free(message);

#ifdef 		DEBUG_MODE

    std::cout << "Sended " << bytes_send << " bytes to " << client->getFd() << ", " << size - bytes_send << " left" << std::endl;

#endif

}

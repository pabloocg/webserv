#include "ServerC.hpp"

void http::ServerC::read_request(char *buf, std::vector<http::Client>::iterator &client)
{
    std::string s_buffer;
    ssize_t size;
    char *message;

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
        http::Request req(s_buffer, get_server(), this->_bad_request, this->_env);
        if (!(message = req.build_response(&size, _mime_types)))
            throw ServerError("request", "failed for some reason");
        client->setSending(true);
        client->setupSend(message, size, 0, size);
        FD_SET(client->getFd(), &_master_write);
    }
}

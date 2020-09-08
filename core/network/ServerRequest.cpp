#include "ServerC.hpp"

void http::ServerC::read_request(char *buf, std::vector<http::Client>::iterator &client, int valread)
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
    if (client->read_valid_format(buf, valread))
    {
        client->setReading(false);
        s_buffer = client->getHeaders() + client->getMessage();
        this->_status_code = client->getCodeStatus();
        this->_host_header = client->getHostHeader();
        try
        {
            http::Request req(s_buffer, get_server(), this->_status_code, this->_env, client->get_dechunked_body());

            if (!(message = req.build_response(&size, _mime_types)))
                throw 500;
        }
        catch (const int internal_error) //Cath error 503 Internal Server Error
        {
            http::Request	req(internal_error);

            if (!(message = req.build_response(&size, _mime_types)))
                throw ServerError("request", "failed for some reason");
        }
		client->reset_read();
        client->setSending(true);
        client->setupSend(message, size, 0, size);
        FD_SET(client->getFd(), &_master_write);
    }
}

#include "Request.hpp"
#include "../utils/base64.hpp"

void http::Request::decode_chunked(void)
{
	int i = 0;
	int ret = 0;
    int end_headers;
	int body_length = 0;
	std::string tmp;
	std::vector<std::string> chunked_vec;

    if ((end_headers = this->_request.find("\r\n\r\n")) != std::string::npos)
		chunked_vec = http::split(this->_request.substr(end_headers + 4), '\n');
    else
        return ;
	while ((ret = stoi(chunked_vec[i], 0, 16)) > 0)
	{
		body_length += ret;
		this->_request_body += chunked_vec[++i].substr(0, ret);
		i++;
	}
	this->_req_content_length = std::to_string(body_length);
}

void http::Request::get_languages_vector(void)
{
    int min;

    this->_languages_accepted = http::split(this->_language_header, ',');
    for (int i = 0; i < (int)this->_languages_accepted.size(); i++)
    {
        min = 0;

        #ifdef DEBUG_MODE

        std::cout << "language antes de tratarlo: " << this->_languages_accepted[i] << std::endl;

        #endif

        if (this->_languages_accepted[i][0] == ' ')
            min = 1;
        if (this->_languages_accepted[i].find(';') != std::string::npos)
            this->_languages_accepted[i] = this->_languages_accepted[i].substr(min, this->_languages_accepted[i].find(';'));
        else
            this->_languages_accepted[i] = this->_languages_accepted[i].substr(min);

        #ifdef DEBUG_MODE

        std::cout << "language: " << this->_languages_accepted[i] << std::endl;

        #endif
    }
}

std::string http::Request::custom_header_to_env(std::string custom_header)
{
    size_t i;
    std::string key;
    std::string value;

    i = custom_header.find(':');
    key = custom_header.substr(0, i);
    value = custom_header.substr(i + 1);
    for (int i = 0; i < (int)key.length(); i++)
    {
        if (key[i] >= 'a' && key[i] <= 'z')
            key[i] = std::toupper(key[i]);
        if (key[i] == '-')
            key[i] = '_';
    }
    key += "=";
    for (int i = 0; i < (int)value.length(); i++)
        if (value[i] != ' ' && value[i] != '\r')
            key += value[i];

#ifdef DEBUG_MODE

    std::cout << "adding env var: " << key << std::endl;

#endif
    return (key);
}

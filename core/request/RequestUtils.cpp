#include "Request.hpp"

void		http::Request::prepare_status()
{
	this->_message_status = this->_error_mgs[this->_status];
	if (this->_status >= 400)
	{
		this->_isCGI = false;
		this->_file_req = this->_server.getErrorPage(this->_status);
		this->_file_type = this->_file_req.substr(_file_req.find(".") + 1);
	}
}

std::string http::Request::get_content_type(std::string type, std::map<std::string, std::string> mime_types)
{
	std::map<std::string, std::string>::iterator iter = mime_types.find(type);

	if (iter == mime_types.end())
		return ("application/octet-stream");
	else
		return (iter->second);
}

void http::Request::decode_chunked(void)
{
	int ret = 0;
	std::string tok;
	char *dechunked;
	dechunked = (char *)malloc(10000);
    if (!dechunked)
        throw 500;
	int dechunked_capacity = 10000;
	int dechunked_size = 0;
	std::cout << "va a dar segfault" << std::endl;
	std::stringstream ss(this->_request.substr(this->_request.find("\r\n\r\n") + 4));
	while (std::getline(ss, tok, '\n'))
	{
		if ((ret = std::stoi(tok, 0, 16)) == 0)
			break;
		std::getline(ss, tok, '\n');
		if (dechunked_capacity < dechunked_size + ret){
			char * tmp = (char *)malloc(dechunked_size + ret + 10000000);
			memcpy(tmp, dechunked, dechunked_size);
			free(dechunked);
			dechunked_capacity = dechunked_size + ret + 10000000;
		}
		memcpy(dechunked + dechunked_size, tok.c_str(), ret);
	}
	this->_request_body = std::string(dechunked);
	this->_req_content_length = std::to_string(dechunked_size);
	//free(dechunked);
	std::cout << "nooo segfault" << std::endl;

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

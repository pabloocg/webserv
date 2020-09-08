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
	else{
		if (iter->second.find("text") != std::string::npos){
			return (iter->second + "; charset=" + this->_charset);
		}
		return (iter->second);
	}
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

void http::Request::get_charset(void){
	this->_charset = "iso-8859-1";
	if (this->_isCGI){
		for (int i = 0; i < (int)this->_CGI_headers.size(); i++){
			if (this->_CGI_headers[i].find("Charset=") != std::string::npos){
				this->_charset = this->_CGI_headers[i].substr(this->_CGI_headers[i].find("Charset=") + 8);
				if (this->_charset.back() == '\r'){
					this->_charset.erase(this->_charset.back());
				}
			}
		}
	}
	else{
		if (this->_resp_body.find("charset=") != std::string::npos){
			int pos = this->_resp_body.find("charset=") + 8;
			if (this->_resp_body[pos] == '\"'){
				int count = 0;
				int i = pos;
				while(this->_resp_body[++i] != '\"'){
					count++;
				}
				this->_charset = this->_resp_body.substr(pos + 1, count);
			}
			else{
				int i = pos;
				int count = 0;
				while(this->_resp_body[i] != '\r' && this->_resp_body[i] != ' ' && this->_resp_body[i] != '\n' && this->_resp_body[i] != ';' && this->_resp_body[i] != ',' && this->_resp_body[i] != '\"'){
					count++;
					i++;
				}
				this->_charset = this->_resp_body.substr(pos, count);
			}
		}
	}
	for(int i = 0; i < (int)this->_charset.length(); i++){
		if (this->_charset[i] >= 'A' && this->_charset[i] <= 'Z'){
			this->_charset[i] = std::tolower(this->_charset[i]);
		}
	}
	if (this->_charset_header.size() > 0){

		bool accepted = false;
		std::cout << "header charset: " << this->_charset_header << std::endl;
		std::vector<std::string> charsets_accepted = http::split(this->_charset_header, ',');
		for (int i = 0; i < (int)charsets_accepted.size(); i++)
		{
			std::cout << "va a comparar " << this->_charset << " con " << charsets_accepted[i] << std::endl;
			if (charsets_accepted[i].find(this->_charset) != std::string::npos){
				accepted = true;
				break;
			}
		}
		if (!accepted){
			this->_status = 406;
			prepare_status();
			this->_resp_body = http::file_content(this->_file_req);
		}
		else{
			this->_set_content_location = true;
		}
	}
	std::cout << "CHARSET: " << this->_charset << std::endl;
}

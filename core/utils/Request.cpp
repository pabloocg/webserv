#include "Request.hpp"
#include "base64.hpp"

void http::Request::save_header(std::string header)
{
	std::vector<std::string> words;
	words = http::split(header, ' ');

	if (words[0] == "Accept-Charsets:")
	{
	}
	else if (words[0] == "Accept-Language:")
	{
	}
	else if (words[0] == "Allow:")
	{
	}
	else if (words[0] == "Authorization:")
	{
		this->_auth = words[2];
	}
	else if (words[0] == "Content-Language:")
	{
	}
	else if (words[0] == "Content-Length:")
	{
	}
	else if (words[0] == "Content-Location:")
	{
	}
	else if (words[0] == "Content-Type:")
	{
	}
	else if (words[0] == "Date:")
	{
	}
	else if (words[0] == "Host:")
	{
	}
	else if (words[0] == "Location:")
	{
	}
	else if (words[0] == "Referer:")
	{
	}
	else if (words[0] == "Retry-After:")
	{
	}
	else if (words[0] == "Server:")
	{
	}
	else if (words[0] == "Transfer-Encoding:")
	{
	}
	else if (words[0] == "User-Agent:")
	{
	}
	else if (words[0] == "WWW-Authenticate:")
	{
	}
}

http::Request::Request(std::string req, http::ServerConf server): _error_mgs(create_map())
{
	/*std::cout << "************ REQUEST ************" << std::endl;
	std::cout << req << std::endl;
	std::cout << "*********************************" << std::endl;*/
	this->request = req;
	this->_server = server;

	std::vector<std::string> sheader;
	std::vector<std::string> srequest;

	sheader = http::split(req, '\n');

	srequest = http::split(sheader[0], ' ');
	if (srequest[0] == "GET")
		this->type = GET;
	else if (srequest[0] == "HEAD")
		this->type = HEAD;
	else if (srequest[0] == "OPTIONS")
		this->type = OPTIONS;
	else if (srequest[0] == "POST")
		this->type = POST;
	else if (srequest[0] == "PUT")
		this->type = PUT;

	this->file_req = srequest[1];
	this->location = this->_server.getRoutebyPath(this->file_req);
	this->file_req = this->location.getFileTransformed(this->file_req);
	this->file_type = this->file_req.substr(file_req.find(".") + 1, file_req.size());
	this->http_version = srequest[2];
	this->_auth = "NULL";
	this->_www_auth_required = false;
	for (int i = 1; i < (int)sheader.size(); i++)
	{
		this->save_header(sheader[i]); //esto leera cada header y lo guardara en su respectiva variable privada de Request
	}
}

std::string http::Request::get_content_type(std::string type, std::map<std::string, std::string> mime_types)
{
	std::map<std::string, std::string>::iterator iter = mime_types.find(type);
	if (iter == mime_types.end())
	{
		return ("application/octet-stream");
	}
	else
	{
		return (iter->second);
	}
}

bool http::Request::needs_auth(http::Routes route)
{
	if (route.needAuth())
	{
		this->_realm = route.getAuthMessage();
		this->_auth_file_path = route.getPassAuthFile();
		return (true);
	}
	else if (!route.needExplicitAuth() && this->_server.needAuth())
	{
		this->_realm = this->_server.getAuthMessage();
		this->_auth_file_path = this->_server.getPassAuthFile();
		return (true);
	}
	return (false);
}

void http::Request::read_file_requested(void)
{
	std::ifstream		file;
	std::stringstream	stream;
	std::string			buf;
	int					code;

	code = 0;
	if (needs_auth(this->location))
	{
		if (this->_auth == "NULL")
		{
			code = 401;
			this->_www_auth_required = true;
		}
		else if (!validate_password(this->_auth))
			code = 403;
	}
	if (!http::file_exists(this->file_req))
	{
		if (code == 0)
			code = 200;
	}
	else
		code = 404;
	this->status = code;
	this->message_status = this->_error_mgs[code];
	if (this->status != 200)
	{
		this->file_req = this->_server.getErrorPage(code);
		this->file_type = this->file_req.substr(file_req.find(".") + 1);
	}
	file.open(this->file_req);
	if (file.is_open())
	{
		while (std::getline(file, buf))
			stream << buf << "\n";
		this->resp_body = stream.str();
		file.close();
	}
	else
		throw "SERVER ERROR";
}

char *http::Request::build_get(int *size, std::map<std::string, std::string> mime_types)
{
	char *res;
	std::string buf;
	std::stringstream stream;
	
	read_file_requested();
	stream << "HTTP/1.1 " << this->status << " " << this->message_status << "\nContent-Type: "
		   << get_content_type(this->file_type, mime_types) << "\nContent-Length: " << this->resp_body.length();
	if (this->_www_auth_required == true)
		stream << "\nWWW-Authenticate: Basic realm=\"" << this->_realm << "\"";
	stream << "\n\n";
	if (this->type == GET)
		stream << this->resp_body;

	this->resp_body = stream.str();
	/*std::cout << "************ RESPONSE ***********" << std::endl;
	std::cout << this->resp_body << std::endl;
	std::cout << "*********************************" << std::endl;*/
	if (!(res = (char *)malloc(sizeof(char) * (this->resp_body.size() + 1))))
		return (NULL);
	std::copy(this->resp_body.begin(), this->resp_body.end(), res);
	res[this->resp_body.size()] = '\0';
	*size = this->resp_body.size();
	std::cout << "Sending " << this->file_req << std::endl;
	return (res);
}

char *http::Request::build_response(int *size, std::map<std::string, std::string> mime_types)
{
	if (this->type == GET || this->type == HEAD)
	{
		return (this->build_get(size, mime_types));
	}
	return (NULL);
}

bool http::Request::validate_password(std::string auth)
{
	std::ifstream htpasswd;
	std::string buf;

	auth = auth.substr(0, auth.size() - 1);
	std::vector<std::uint8_t> decoded = base64::decode(auth);
	std::string decoded_str;
	for (int i = 0; i < (int)decoded.size(); i++)
	{
		decoded_str += (char)decoded[i];
	}
	int pos = decoded_str.find(':');
	std::string user;
	std::string password;
	user = decoded_str.substr(0, pos);
	password = decoded_str.substr(pos + 1, (decoded_str.size() - pos + 1));
	htpasswd.open(this->_auth_file_path);
	if (htpasswd.is_open())
	{
		while (std::getline(htpasswd, buf))
		{
			if (user == buf.substr(0, buf.find(':')))
			{
				std::string encrypted_passwd = buf.substr(buf.find(':') + 1, buf.size() - buf.find(':') + 1);
				decoded.clear();
				for (int i = 0; i < (int)password.size(); i++)
				{
					decoded.push_back((unsigned char)password[i]);
				}
				std::string encoded = base64::encode(decoded);
				if (encoded == encrypted_passwd)
				{
					return (true);
				}
			}
		}
	}
	return (false);
}

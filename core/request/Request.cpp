#include "Request.hpp"
#include "../utils/base64.hpp"

http::Request::Request(std::string req, http::ServerConf server, bool bad_request, std::vector<std::string> env):
						_is_autoindex(false),
						_www_auth_required(false),
						_isCGI(false),
						_request(req),
						_auth("NULL"),
						_server(server),
						_error_mgs(create_map()),
						_env(env),
						_status(0)
{

#ifdef 		DEBUG_MODE

	std::cout << "************ REQUEST HEADERS ************" << std::endl;
	std::cout << req.substr(0, req.find("\r\n\r\n")) << std::endl;
	std::cout << "*********************************" << std::endl;

#endif

	this->_allow.clear();
	if (bad_request == true)
		this->_status = 400;
	this->save_request();
	if (!this->_is_autoindex && this->_location.isCgi() && this->_type != OPTIONS && !this->_location.isRedirect())
	{
		this->_isCGI = true;

		#ifdef 		DEBUG_MODE

		std::cout << "extension =" << this->_location.getExtension() << std::endl;

		#endif

		this->_path_info = this->_file_req.substr(this->_file_req.find(this->_location.getExtension()) + 3);
		this->_script_name = this->_file_req.substr(0, this->_file_req.find(this->_location.getExtension()) + 3);
		this->_file_req = this->_script_name;
		add_basic_env_vars();

		#ifdef 		DEBUG_MODE

		std::cout << "File after CGI Transformation: " << this->_file_req << std::endl;

		#endif
	}
	if (!this->_location.isRedirect())
	{
		if (!http::is_dir(this->_file_req) || this->_type == PUT)
			this->_file_type = this->_file_req.substr(_file_req.find(".") + 1, _file_req.size());
		else if (this->_location.getAutoIndex())
			this->_is_autoindex = true;
	}
	set_status();
	prepare_status();
}

void	http::Request::save_request()
{
	std::vector<std::string> sheader;
	std::vector<std::string> srequest;

	sheader = http::split(this->_request.substr(0, this->_request.find("\r\n\r\n")), '\n');
	srequest = http::split(sheader[0], ' ');
	if (srequest[0] == "GET")
		this->_type = GET;
	else if (srequest[0] == "HEAD")
		this->_type = HEAD;
	else if (srequest[0] == "OPTIONS")
		this->_type = OPTIONS;
	else if (srequest[0] == "POST")
		this->_type = POST;
	else if (srequest[0] == "PUT")
		this->_type = PUT;
	else if (srequest[0] == "DELETE")
		this->_type = DELETE;
	this->_req_URI = srequest[1];
	this->_http_version = srequest[2];
	for (int i = 1; i < (int)sheader.size(); i++)
		this->save_header(sheader[i]);
	if (atoi(this->_req_content_length.c_str()) > 0)
		save_request_body();
	if (this->_transf_encoding.find("chunked") != std::string::npos)
		decode_chunked();
	get_languages_vector();
	if (this->_req_URI.find('?') != std::string::npos)
	{
		this->_query_string = this->_req_URI.substr(this->_req_URI.find("?") + 1);
		this->_file_bef_req = this->_req_URI.substr(0, this->_req_URI.find("?"));
	}
	else
		this->_file_bef_req = this->_req_URI;
	if (this->_file_bef_req.find(".") == std::string::npos && this->_file_bef_req.back() != '/' && this->_type != PUT && this->_type != POST)
		this->_file_bef_req += '/';
	this->_location = this->_server.getRoutebyPath(this->_file_bef_req);

	#ifdef 		DEBUG_MODE

	std::cout << "Location: " << this->_location.getVirtualLocation() << std::endl;

	#endif

	if (this->_location.isRedirect())
		this->_file_bef_req = this->_location.getPathRedirect();
	else
	{
		#ifdef 		DEBUG_MODE

		std::cout << "File before getFileTransformed: " << this->_file_bef_req << std::endl;

		#endif

		this->_file_req = this->_location.getFileTransformed(this->_file_bef_req, this->_languages_accepted, this->_type, this->_language_setted);
		
		#ifdef 		DEBUG_MODE
		
		std::cout << "File after getFileTransformed: " << this->_file_req << std::endl;
		
		#endif
	}
}

void http::Request::save_header(std::string header)
{
	std::vector<std::string> words = http::split(header, ' ');

	if (words[0] == "Accept-Charsets:")
	{
	}
	if (words[0] == "Accept-Encoding:")
	{
	}
	else if (words[0] == "Accept-Language:")
		this->_language_header = header.substr(16, header.length() - 17);
	else if (words[0] == "Authorization:")
		this->_auth = words[2];
	else if (words[0] == "Content-Location:")
	{
	}
	else if (words[0] == "Content-Type:")
		this->_req_content_type = words[1];
	else if (words[0] == "Content-Length:")
		this->_req_content_length = header.substr(16, header.length() - 17);
	else if (words[0] == "Location:")
	{
	}
	else if (words[0] == "Referer:" || words[0] == "User-Agent:")
	{
		for (int i = 0; i < (int)words.size(); i++)
			this->_client_info += words[i];
		this->_client_info += "\n";
	}
	else if (words[0] == "Transfer-Encoding:")
		this->_transf_encoding = words[1];
	else if (words[0] == "Host:")
	{
	}
	else if (words[0][0] == 'X' && words[0][1] == '-')
	{
		std::cout << "adds custom header: " << header << std::endl;
		this->_custom_headers.push_back(header);
	}
}

void http::Request::save_request_body(void)
{
	for (int i = 0; i < (int)this->_request.length(); i++)
		if (this->_request[i] == '\n' && this->_request[i + 2] == '\n')
			this->_request_body = this->_request.substr(i + 3, atoi(this->_req_content_length.c_str()));
	
}

void http::Request::set_status(void)
{
	int code = 0;

	if (!this->_http_version.compare("HTTP/1.1"))
		this->_status = 505;
	if (!this->_status && this->_location.MethodAllow(this->_type))
	{
		if (needs_auth(this->_location))
		{
			if (this->_auth == "NULL")
			{
				code = 401;
				this->_www_auth_required = true;
			}
			else if (!validate_password(this->_auth))
				code = 403;
		}
		if (this->_location.isRedirect()){
			code = this->_location.getCodeRedirect();
		}
		else if (this->_request_body.length() > this->_location.getBodySize() && !code)
			code = 413;
		else if (!http::file_exists(this->_file_req) && !http::is_dir(this->_file_req))
		{
			if (!code)
			{
				if (this->_type == OPTIONS)
					code = 204;
				else
					code = 200;
			}
		}
		else if (http::file_exists(this->_file_req) && (this->_type == PUT or (this->_type == POST && !this->_isCGI)) && !code){
			code = 201;
		}
		else if (http::file_exists(this->_file_req) && this->_type == POST && this->_isCGI && !code){
			code = 200;
		}
		//else if (http::is_dir(this->_file_req))
		//	code = 403;
		else
		{
			code = 404;
			this->_is_autoindex = false;
		}
	}
	else if (!this->_status)
	{
		code = 405;
		get_allowed_methods();
	}
	if (!this->_status)
		this->_status = code;
}

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

char *http::Request::getResponse(int *size, std::map<std::string, std::string> mime_types)
{
	char *res;
	std::stringstream stream;

	stream << "HTTP/1.1 " << this->_status << " " << this->_message_status;
	if (!this->_isCGI && this->_status != 204 && this->_type != PUT && !this->_is_autoindex)
		stream << "\nContent-Type: " << get_content_type(this->_file_type, mime_types);
	else if (this->_is_autoindex)
		stream << "\nContent-Type: text/html";
	if (this->_status == 405 || (this->_status == 204 && this->_type == OPTIONS))
	{
		stream << "\nAllow:";
		for (int i = 0; i < (int)this->_allow.size(); i++)
		{
			if (i < (int)this->_allow.size() - 1)
				stream << " " << this->_allow[i] << ",";
			else
				stream << " " << this->_allow[i];
		}
	}
	if (this->_www_auth_required == true)
		stream << "\nWWW-Authenticate: Basic realm=\"" << this->_realm << "\"";
	if (this->_isCGI)
		for (int i = 0; i < (int)this->_CGI_headers.size(); i++)
			stream << "\n" << this->_CGI_headers[i];
	stream << "\nDate: " << http::get_actual_date();
	if (this->_status != 204 && this->_type != PUT)
		stream << "\nContent-Length: " << this->_resp_body.length();
	else
		stream << "\nContent-Length: 0";
	if (this->_language_setted.length() > 0)
		stream << "\nContent-Language: " << this->_language_setted;
	if (this->_status == 201 or this->_status == 301 or this->_status == 302)
		stream << "\nLocation: " << this->_file_bef_req;
	stream << "\nServer: Webserv/1.0\r\n\r\n";
	if (this->_type != HEAD && this->_status != 204 && this->_type != PUT)
		stream << this->_resp_body;
	this->_resp_body = stream.str();
	if (this->_resp_body.length() < 1000)
	{
		std::cout << "************ RESPONSE ***********" << std::endl;
		std::cout << this->_resp_body;
		std::cout << "*********************************" << std::endl;
	}
	if (!(res = (char *)malloc(sizeof(char) * (this->_resp_body.size() + 1))))
		return (NULL);
	std::copy(this->_resp_body.begin(), this->_resp_body.end(), res);
	res[this->_resp_body.size()] = '\0';
	*size = this->_resp_body.size();
	std::cout << "Sending " << this->_file_req << std::endl;
	return (res);
}

bool http::Request::validate_password(std::string auth)
{
	std::ifstream htpasswd;
	std::string buf;
	std::string decoded_str;
	std::string user;
	std::string password;

	auth = auth.substr(0, auth.size() - 1);
	std::vector<std::uint8_t> decoded = base64::decode(auth);
	for (int i = 0; i < (int)decoded.size(); i++)
		decoded_str += (char)decoded[i];
	int pos = decoded_str.find(':');
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
					decoded.push_back((unsigned char)password[i]);
				std::string encoded = base64::encode(decoded);
				if (encoded == encrypted_passwd)
					return (true);
			}
		}
	}
	return (false);
}

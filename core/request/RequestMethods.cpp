#include "Request.hpp"

char *http::Request::build_response(ssize_t *size, std::map<std::string, std::string> mime_types)
{
	if (this->_type == GET || this->_status >= 400)
		this->build_get();
	else if (this->_type == POST)
		this->build_post();
	else if (this->_type == PUT && !this->_isCGI)
		this->build_put();
	else if (this->_type == DELETE)
		this->build_delete();
	else if (this->_type == OPTIONS)
		this->build_options();
	if (this->_status >= 500)
	{
		prepare_status();
		this->_resp_body = http::file_content(this->_file_req);
	}
	return (getResponse(size, mime_types));
}

void http::Request::build_get(void)
{
	if (this->_status != 204 && this->_status != 201 && !this->_isCGI)
	{
		if (this->_is_autoindex)	
			this->_resp_body = build_autoindex();
		else
			this->_resp_body = http::file_content(this->_file_req);
	}
	if (!this->_is_autoindex && this->_isCGI)
		startCGI();
}

void http::Request::build_post(void)
{
	if (this->_isCGI)
		startCGI();
	else
	{
		std::ofstream f(this->_file_req, std::ios::app);

		if (f.good())
		{
			f << this->_request_body << std::endl;
			f.close();
		}
	}
}

void http::Request::build_put(void)
{
	std::ofstream f(this->_file_req);

	if (f.good())
	{
		f << this->_request_body << std::endl;
		f.close();
	}
	else
		throw 500;
}

void http::Request::build_delete(void)
{
	std::stringstream stream;

	unlink(this->_file_req.c_str());
	stream << "<html>\n<body>\n<h1>File deleted.</h1>\n</body>\n</html>" << std::endl;
	this->_resp_body = stream.str();
}

void http::Request::build_options(void)
{
	get_allowed_methods();
}

void http::Request::get_allowed_methods(void)
{
	if (this->_location.MethodAllow(GET))
		this->_allow.push_back("GET");
	if (this->_location.MethodAllow(HEAD))
		this->_allow.push_back("HEAD");
	if (this->_location.MethodAllow(POST))
		this->_allow.push_back("POST");
	if (this->_location.MethodAllow(PUT))
		this->_allow.push_back("PUT");
	if (this->_location.MethodAllow(OPTIONS))
		this->_allow.push_back("OPTIONS");
	if (this->_location.MethodAllow(DELETE))
		this->_allow.push_back("DELETE");
	if (this->_location.MethodAllow(CONNECT))
		this->_allow.push_back("CONNECT");
	if (this->_location.MethodAllow(TRACE))
		this->_allow.push_back("TRACE");
}

std::string	http::Request::build_autoindex(void)
{
	std::string			tmp;
	std::string			tmp_p;
	std::stringstream	stream;
	char				buftime[30];
	DIR					*dirp;
	struct dirent		*direntp;
	struct tm			*gm;
	struct stat			buff;

	stream << "<html>\n<head><title>Index of " << this->_file_bef_req << "</title></head><body>\n";
	stream << "<h1>Index of " << this->_file_bef_req << "</h1><hr><pre><br>" << std::endl;
	dirp = opendir(this->_file_req.c_str());
	if (dirp == NULL)
		throw 500;
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (!strncmp(direntp->d_name, ".", strlen(direntp->d_name)))
			continue;
		tmp = this->_file_bef_req + direntp->d_name;
		tmp_p = this->_file_req + direntp->d_name;
		if (!strncmp(direntp->d_name, "..", strlen(direntp->d_name)))
			stream << "<a href=\"" << tmp << "\">" << direntp->d_name << "/</a>";
		else
		{
			stream << "<a href=\"" << tmp << "\">" << direntp->d_name << "</a>";
			stat(tmp_p.c_str(), &buff);
			ssize_t written = -1;
			gm = gmtime(&buff.st_mtimespec.tv_sec);
			if (gm)
				if (!((written = (ssize_t)strftime(buftime, sizeof(buftime), "%d-%b-%Y %H:%M", gm)) > 0))
					throw 500;
			for (size_t i = 0; i < 20; i++)
				stream << "&nbsp;";
			stream << buftime;
			for (size_t i = 0; i < 8; i++)
				stream << "&nbsp;";
			if (S_ISDIR(buff.st_mode))
				stream << "-";
			else
				stream << http::file_size(tmp_p);
		}
		stream << "<br>" << std::endl;
	}
	closedir(dirp);
	stream << "</pre><hr></body>\n</html>\n" << std::endl;
	return (stream.str());
}

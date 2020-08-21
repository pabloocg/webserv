#include "Conf.hpp"

http::Conf::Conf(const std::string &conf_file):
            _filename(conf_file),
            _log(DEFAULT_ACCESS_LOG, DEFAULT_ERROR_LOG)
{
    if (!file_exists())
    {
        this->_log.makeLog(ERROR_LOG, "The entered configuration file doesn't exist");
        exit(EXIT_FAILURE);
    }
    complex_parse(simple_parse());
}

bool        http::Conf::file_exists(void)
{
    this->_file.open(this->_filename);
    return (this->_file.is_open());
}

std::string        http::Conf::simple_parse(void)
{
    std::string     strr;
    size_t          pos;
    std::regex      comment("\\s*[#].*");
    std::stringstream   buf;

    while (std::getline(this->_file, strr))
    {
        if (!std::regex_match(strr, comment) and strr.length() > 0)
        {
            if ((pos = strr.find("#")) != std::string::npos)
                strr = strr.substr(0, pos);
            buf << http::trim(strr);
        }
    }
    this->_file.close();
    return (buf.str());
}

http::Routes    http::Conf::save_location(std::string s)
{
    http::Routes        route;
    std::stringstream   buf;
    std::vector<std::string>    params;
    size_t              count = 0;

    s.erase(0, 9);
    while (std::isspace(s[count]))
        count++;
    while (!std::isspace(s[count]))
        buf << s[count++];
    route.setLocation(buf.str()); // Save virtual directory location
    while (std::isspace(s[count]))
        count++;
    if (!(s[count] == '{'))
        throw Conf::UnrecognizedParameter();
    s = s.substr(count + 1, s.find_last_of("}") - (count + 1));
    buf.clear();
    buf.str("");
    //std::cout << s << std::endl;
    params = http::special_split(s, ';');
    for (size_t i = 0; i < params.size(); i++)
    {
        //std::cout << params[i] << std::endl;
        if (!params[i].compare(0, 5, "root ")) // Existing directory path
            route.setDirPath(params[i].substr(params[i].find_last_of(' ') + 1));
        else if (!params[i].compare(0, 6, "index ")) // File index
            route.setIndexFile(params[i].substr(params[i].find_last_of(' ') + 1));
        else if (!params[i].compare(0, 10, "autoindex ")) // Activate autoindex (show files in a dir)
        {
            if (params[i].substr(params[i].find_last_of(' ') + 1) == "on")
                route.allowAutoIndex();
        }
        else if (!params[i].compare(0, 7, "upload ")) // allow uploads files
        {
            if (params[i].substr(params[i].find_last_of(' ') + 1) == "on")
                route.allowUpload();
        }
        else if (!params[i].compare(0, 12, "path_upload ")) // Path to save uploads
            route.setUploadPath(params[i].substr(params[i].find_last_of(' ') + 1));
        else if (!params[i].compare(0, 13, "http_methods ")) // methods allowed in a location
        {
            count = 13;
            std::string     s_cmp;
            std::string     to_str(params[i]);
            while (count < to_str.length())
            {
                while (std::isspace(to_str[count]) && count < to_str.length())
                    count++;
                while (!std::isspace(to_str[count]) && count < to_str.length())
                    buf << to_str[count++];
                s_cmp = buf.str();
                if (s_cmp != "GET" && s_cmp != "POST" && s_cmp != "HEAD" && s_cmp != "DELETE"
                    && s_cmp != "CONNECT" && s_cmp != "OPTIONS" && s_cmp != "TRACE" && s_cmp != "PATCH")
                    throw http::Conf::UnrecognizedParameter();
                route.setMethods(s_cmp);                
                buf.clear();
                buf.str("");
            }
        }
        else
            throw Conf::UnrecognizedParameter();
    }
    return (route);
}

void        http::Conf::parse_types(std::string s)
{
	std::string value;
	int i = 0;
    int count;
	while (i < (int)s.length())
    {
        count = 0;
		while (!std::isspace(s[i + count]))
			count++;
		value = s.substr(i, count);
		//std::cout << "value added: " << value << std::endl;
		i += count;
		while (s[i] != ';')
        {
			while (std::isspace(s[i]))
				i++;
			count = 0;
			while (!std::isspace(s[i + count]) && s[i + count] != ';')
				count++;
			std::pair<std::string, std::string> pair(s.substr(i, count), value);
			this->_mime_types.insert(pair);
			//std::cout << "ha insertado " << s.substr(i, count) << std::endl;
			i += count;
		}
		i++;
	}
}

void        http::Conf::parse_server_conf(std::string s)
{
    http::ServerConf    new_server;
    //std::cout << "SERVER" << std::endl;
    //std::cout << s << std::endl;

    std::vector<std::string>    params;

    params = http::special_split(s, ';');

    for (size_t i = 0; i < params.size(); i++)
    {
        //std::cout << params[i] << std::endl;
        // Read Port
        if (!params[i].compare(0, 7, "listen "))
            new_server.setPort(std::atoi(params[i].substr(params[i].find_last_of(' ') + 1).c_str()));
        // Read Address
        else if (!params[i].compare(0, 12, "server_addr "))
            new_server.setServerAddr(params[i].substr(params[i].find_last_of(' ') + 1));
        // Read ServerName
        else if (!params[i].compare(0, 12, "server_name "))
            new_server.setServerName(params[i].substr(params[i].find_last_of(' ') + 1));
        // Read BodySize
        else if (!params[i].compare(0, 21, "client_max_body_size "))
            new_server.setBodySize(std::atoi(params[i].substr(params[i].find_last_of(' ') + 1).c_str()));
        // Read Error Pages
        else if (!params[i].compare(0, 11, "error_page "))
            new_server.setErrorPage(params[i].substr(params[i].find_first_of(' ')));
        // Add location
        else if (!params[i].compare(0, 9, "location "))
            new_server.add_route(this->save_location(params[i]));
        else
            throw Conf::UnrecognizedParameter();
    }
    this->_servers.push_back(new_server);
}

size_t      get_BracketClose(std::string s)
{
    size_t i = 0;
    int     level = 0;

    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '{')
            level++;
        else if (s[i] == '}')
        {
            level--;
            if (!level)
                return (i);
        }
    }
    return (i);
}

void        http::Conf::complex_parse(std::string s)
{
    size_t      i = 0;
    size_t      bclose = 0;
    std::string string_p;

    if (std::count(s.begin(), s.end(), '{') != std::count(s.begin(), s.end(), '}'))
        throw Conf::UnclosedBracket();
    if (!s.compare(0, 4, "http"))
        std::cout << "namespace http found" << std::endl;
    else
        throw Conf::UnrecognizedParameter();
    i = 4;
    while (std::isspace(s[i]))
        i++;
    if (!(s[i] == '{'))
        throw Conf::UnrecognizedParameter();
    string_p = s.substr(i + 1, s.find_last_of("}") - (i + 1));
    while (!string_p.empty())
    {
        if (!string_p.compare(0, 6, "server"))
        {
            //std::cout << "namespace server found" << std::endl;
            i = 6;
            while (std::isspace(string_p[i]))
                i++;
            if (!(string_p[i] == '{'))
                throw Conf::UnrecognizedParameter();
            bclose = get_BracketClose(string_p);
            this->parse_server_conf(string_p.substr(i + 1, bclose - (i + 1)));
            string_p = string_p.substr(bclose + 1);
        }
		else if (!string_p.compare(0, 7, "include"))
        {
			//std::cout << "include found" << std::endl;
			i = 7;
			while (std::isspace(string_p[i]))
                i++;
			int aux = 0;
			while (std::isalnum(string_p[i + aux]) || string_p[i + aux] == '.')
				aux++;
			this->_filename = string_p.substr(i, aux);
			//std::cout << "file included: " << this->_filename << std::endl;
			if (!file_exists())
    		{
        		this->_log.makeLog(ERROR_LOG, "Included file at configuration file doesn't exist");
        		exit(EXIT_FAILURE);
    		}
			i += aux;
			while (string_p[i] != ';')
				i++;
			string_p = string_p.substr(i + 1);
			i = 0;
			std::string include = simple_parse();
			string_p = include + string_p;
		}
		else if (!string_p.compare(0, 5, "types"))
        {
			i = 5;
			while (std::isspace(string_p[i]))
                i++;
			if (!(string_p[i] == '{'))
                throw Conf::UnrecognizedParameter();
            bclose = get_BracketClose(string_p);
            this->parse_types(string_p.substr(i + 1, bclose - (i + 1)));
			string_p = string_p.substr(bclose + 1);
		}
        else
            throw Conf::UnrecognizedParameter();
    }
}

std::vector<http::ServerConf>   http::Conf::getServers(void)
{
    return (this->_servers);
}

std::map<std::string, std::string>   http::Conf::get_mime_types(void)
{
	return (this->_mime_types);
}

const char* http::Conf::UnclosedBracket::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Unclosed Bracket");
    return ("File Configuration Exception: Unclosed Bracket");
}

const char* http::Conf::UnrecognizedParameter::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Unrecognized Parameter");
    return ("File Configuration Exception: Unrecognized Parameter");
}

const char* http::Conf::SemicolonMissing::what() const throw()
{
    //this->_log.makeLog(ERROR_LOG, "File Configuration Exception: Semicolon Missing");
    return ("File Configuration Exception: Semicolon Missing");
}

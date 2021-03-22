#include "Conf.hpp"

const std::string http::Conf::_http_methods[9] = {
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH"
};

http::Conf::Conf(const std::string &conf_file) : _filename(conf_file)
{
    if (!file_exists())
        throw ConfError("The configuration file entered does not exist");
    complex_parse(simple_parse());
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        std::vector<http::Routes>   routes(this->_servers[i].getRoutes());
        std::vector<http::Routes>   tmp;
        for (std::vector<http::Routes>::iterator itr = routes.begin(); itr != routes.end(); itr++)
            tmp.push_back(this->check_inheritance(*itr, routes, i));
        this->_servers[i].set_routes(tmp);
    }
}

bool            http::Conf::file_exists(void)
{
    this->_file.open(this->_filename);
    return (this->_file.is_open());
}

std::string     http::Conf::simple_parse(void)
{
    std::string         strr;
    size_t              pos;
    std::regex          comment("\\s*[#].*");
    std::stringstream   buf;

    while (std::getline(this->_file, strr))
        if (!std::regex_match(strr, comment) and strr.length() > 0)
        {
            if ((pos = strr.find("#")) != std::string::npos)
                strr = strr.substr(0, pos);
            buf << http::trim(strr);
        }
    this->_file.close();
    return (buf.str());
}

size_t get_BracketClose(std::string s)
{
    size_t  i = 0;
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

void            http::Conf::complex_parse(std::string s)
{
    size_t              i = 0;
    size_t              bclose = 0;
    std::string         string_p;
    std::string         tmp;
    std::stringstream   buf;

    if (std::count(s.begin(), s.end(), '{') != std::count(s.begin(), s.end(), '}'))
        throw ConfError("Unclosed Bracket");
    if (s.compare(0, 4, "http"))
        throw ConfError("http directive not found");
    i = 4;
    while (std::isspace(s[i]))
        i++;
    if (!(s[i] == '{'))
        throw ConfError(std::string(1, s[i]), "Open Bracket Missing");
    string_p = s.substr(i + 1, s.find_last_of("}") - (i + 1));
    while (!string_p.empty())
    {
        i = 0;
        while (std::isspace(string_p[i]))
            i++;
        while (!std::isspace(string_p[i]) and string_p[i] != '{')
            buf << string_p[i++];
        while (std::isspace(string_p[i]))
            i++;
        tmp = buf.str();
        if (tmp == "server" || tmp == "types")
        {
            if (!(string_p[i] == '{'))
                throw ConfError(std::string(1, s[i]), "Open Bracket Missing");
            bclose = get_BracketClose(string_p);
            if (tmp == "server")
                this->parse_server_conf(string_p.substr(i + 1, bclose - (i + 1)));
            else if (tmp == "types")
                this->parse_types(string_p.substr(i + 1, bclose - (i + 1)));
            string_p = string_p.substr(bclose + 1);
        }
        else if (tmp == "include")
        {
            int aux = 0;
            while (std::isalnum(string_p[i + aux]) || string_p[i + aux] == '.')
                aux++;
            this->_filename = string_p.substr(i, aux);
            if (!file_exists())
                throw ConfError("File " + this->_filename + " don't exists");
            i += aux;
            while (std::isspace(string_p[i]))
                i++;
            if (!(string_p[i] == ';'))
                throw ConfError(std::string(1, string_p[i]), "Semicolon missing");
            string_p = string_p.substr(i + 1);
            string_p = simple_parse() + string_p;
        }
        else
            throw ConfError(tmp, "Unrecognized parameter");
        buf.clear();
        buf.str("");
    }
}

void http::Conf::parse_types(std::string s)
{
    std::string value;
    size_t      i = 0;
    size_t      count;

    while (i < s.length())
    {
        count = 0;
        while (!std::isspace(s[i + count]))
            count++;
        value = s.substr(i, count);
        i += count;
        while (s[i] != ';')
        {
            while (std::isspace(s[i]))
                i++;
            count = 0;
            while (!std::isspace(s[i + count]) && s[i + count] != ';')
                count++;
            this->_mime_types.insert(std::make_pair(s.substr(i, count), value));
            i += count;
        }
        i++;
    }
}

void http::Conf::parse_server_conf(std::string s)
{
    http::ServerConf    new_server;
    std::stringstream   buf;
    std::string         tmp;
    size_t              i = 0;
    size_t              count;

    while (i < s.length())
    {
        count = 0;
        while (std::isspace(s[i]))
            i++;
        while (!std::isspace(s[i]))
            buf << s[i++];
        tmp = buf.str();
        buf.clear();
        buf.str("");
        if (tmp == "listen" or tmp == "server_addr" or tmp == "server_name" or
            tmp == "client_max_body_size" or tmp == "auth_basic_user_file" or tmp == "root")
        {
            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && s[i] != ';')
                buf << s[i++];
            if (buf.str().length() < 1)
                throw ConfError(tmp, "Unrecognized parameter");
            if (tmp == "listen")
            {
                for (size_t i = 0; i < buf.str().length(); i++)
                    if (!std::isdigit(buf.str()[i]))
                        throw ConfError(tmp, "must be a number port valid");
                new_server.setPort(std::atoi(buf.str().c_str()));
                if (buf.str().find("default_server") != std::string::npos)
                    new_server.setDefaultServer();
            }
            else if (tmp == "server_addr")
                new_server.setServerAddr(buf.str());
            else if (tmp == "root")
                new_server.setRootDir(buf.str());
            else if (tmp == "server_name")
                new_server.setServerName(buf.str());
            else if (tmp == "client_max_body_size")
            {
                for (size_t i = 0; i < buf.str().length(); i++)
                    if (!std::isdigit(buf.str()[i]))
                        throw ConfError(tmp, "must be a number");
                new_server.setBodySize(std::atoi(buf.str().c_str()));
            }
            else if (tmp == "auth_basic_user_file")
            {
                if (http::file_exists(buf.str()))
                    throw ConfError(buf.str(), "The include file entered doesn't exist");
                new_server.setPassAuthFile(buf.str());
            }
        }
        else if (tmp == "auth_basic")
        {
            while (std::isspace(s[i]))
                i++;
            while (s[i] != ';')
                buf << s[i++];
            if (buf.str().length() < 1)
                throw ConfError(tmp, "Unrecognized parameter");
            new_server.setAuthMessage(buf.str());
        }
        else if (tmp == "location")
        {
            std::stringstream   mod_opt;
            std::stringstream   vpath;
            http::Routes        new_route;

            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && s[i] != '{')
                mod_opt << s[i++];
            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && s[i] != '{')
                vpath << s[i++];
            while (std::isspace(s[i]))
                i++;
            if (mod_opt.str().length() < 1 || !(s[i] == '{'))
                throw ConfError(std::string(1, s[i]), "Open Bracket Missing");
            count = get_BracketClose(s.substr(i++));
            new_route = this->save_location(s.substr(i, count - 1), mod_opt.str(), vpath.str());
            if (new_server.validLocation(new_route))
                new_server.add_route(new_route);
            else
                throw ConfError(new_route.getVirtualLocation(), "Location cannot be repeated");
            i += count;
            continue ;
        }
        else if (tmp == "error_page")
        {
            std::vector<int>            codes;
            bool                        is_code = true;
            bool                        least = false;

            while (is_code)
            {
                while (std::isspace(s[i]))
                    i++;
                if (std::isdigit(s[i]))
                {
                    least = true;
                    while (std::isdigit(s[i]))
                        buf << s[i++];
                    codes.push_back(atoi(buf.str().c_str()));
                }
                else
                    is_code = false;
                buf.clear();
                buf.str("");
            }
            while (!std::isspace(s[i]) && s[i] != ';')
                buf << s[i++];
            if (!least || http::file_exists(buf.str()))
                throw ConfError(buf.str(), "The error page file entered doesn't exist");
            new_server.setErrorPage(codes, buf.str());
        }
        else if (tmp == "index")
        {
            std::string s_cmp;

            while (i < s.length() && s[i] != ';')
            {
                while (std::isspace(s[i]) && i < s.length())
                    i++;
                while (!std::isspace(s[i]) && i < s.length() && s[i] != ';')
                    buf << s[i++];
                s_cmp = buf.str();
                if (tmp == "index" && s_cmp.substr(s_cmp.find(".") + 1).length() > 13)
                    throw ConfError(s_cmp, "Unrecognized parameter");
                if (tmp == "index")
                    new_server.addIndexFile(s_cmp);
                buf.clear();
                buf.str("");
            }
        }
        else
            throw ConfError(tmp, "Unrecognized parameter");
        buf.clear();
        buf.str("");
        if (s[i] == ';')
            i++;
        else if (std::isspace(s[i]))
        {
            while (std::isspace(s[i]))
                i++;
            if (s[i] == ';')
                i++;
            else
                throw ConfError(tmp, "Semicolon missing");
        }
        else
            throw ConfError(tmp, "Semicolon missing");
    }
    this->_servers.push_back(new_server);
}

http::Routes http::Conf::save_location(std::string s, std::string opt, std::string vpath)
{
    http::Routes        route;
    std::stringstream   buf;
    std::string         tmp;
    size_t              count;
    size_t              i = 0;

    if (vpath == "")
    {
        vpath = opt;
        opt = std::string("");
    }
    route.setOptModifier(opt);
    route.setLocation(vpath);
    while (i < s.length())
    {
        count = 0;
        while (std::isspace(s[i]))
            i++;
        while (!std::isspace(s[i]))
            buf << s[i++];
        tmp = buf.str();
        buf.clear();
        buf.str("");
        if (tmp == "root" or tmp == "autoindex" or tmp == "upload" or tmp == "path_upload"
            or tmp == "auth_basic_user_file" or tmp == "cgi_exec" or tmp == "client_max_body_size")
        {
            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && s[i] != ';')
                buf << s[i++];
            if (buf.str().length() < 1)
                throw ConfError(tmp, "Unrecognized parameter");
            if (tmp == "root" or tmp == "path_upload" or tmp == "auth_basic_user_file" or tmp == "cgi_exec")
            {
                if (http::file_exists(buf.str()))
                    throw ConfError(tmp, "The file " + buf.str() + " entered doesn't exist");
                if (tmp == "root")
                    route.setDirPath(buf.str());
                else if (tmp == "path_upload")
                    route.setUploadPath(buf.str());
                else if (tmp == "auth_basic_user_file")
                    route.setPassAuthFile(buf.str());
                else if (tmp == "cgi_exec")
                    route.setCgiExec(buf.str());
            }
            else if (tmp == "autoindex" or tmp == "upload")
            {
                if (buf.str() != "on" && buf.str() != "off")
                    throw ConfError(tmp, "Unrecognized parameter " + buf.str());
                if (tmp == "autoindex" && buf.str() == "on")
                    route.allowAutoIndex();
                else if (tmp == "upload" && buf.str() == "on")
                    route.allowUpload();
            }
            else if (tmp == "client_max_body_size")
            {
                for (size_t i = 0; i < buf.str().length(); i++)
                    if (!std::isdigit(buf.str()[i]))
                        throw ConfError(tmp, "must be a number");
                route.setBodySize(std::atoi(buf.str().c_str()));
            }
        }
        else if (tmp == "auth_basic")
        {
            while (std::isspace(s[i]))
                i++;
            while (s[i] != ';' && i < s.length())
                buf << s[i++];
            if (buf.str().length() < 1)
                throw ConfError(tmp, "Unrecognized parameter");
            route.setAuthMessage(buf.str());
        }
        else if (tmp == "index" || tmp == "languages")
        {
            std::string s_cmp;

            if (tmp == "languages")
                route.setLanguages();
            while (i < s.length() && s[i] != ';')
            {
                while (std::isspace(s[i]) && i < s.length())
                    i++;
                while (!std::isspace(s[i]) && i < s.length() && s[i] != ';')
                    buf << s[i++];
                s_cmp = buf.str();
                if (tmp == "index" && s_cmp.substr(s_cmp.find(".") + 1).length() > 13)
                    throw ConfError(s_cmp, "Unrecognized parameter");
                if (tmp == "index")
                    route.addIndexFile(s_cmp);
                else if (tmp == "languages")
                    route.addnewLanguage(s_cmp);
                buf.clear();
                buf.str("");
            }
        }
        else if (tmp == "http_methods") // methods allowed in a location
        {
            std::string s_cmp;

            while (i < s.length() && s[i] != ';')
            {
                while (std::isspace(s[i]) && i < s.length())
                    i++;
                while (!std::isspace(s[i]) && i < s.length() && s[i] != ';')
                    buf << s[i++];
                s_cmp = buf.str();
                if (s_cmp != "GET" && s_cmp != "POST" && s_cmp != "HEAD" && s_cmp != "DELETE" &&
                    s_cmp != "OPTIONS" && s_cmp != "PATCH" && s_cmp != "PUT")
                    throw ConfError(tmp, "Unrecognized parameter " + s_cmp);
                route.setMethods(s_cmp);
                buf.clear();
                buf.str("");
            }
        }
        else if (tmp == "return")
        {
            route.setRedirect();
            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && std::isdigit(s[i]))
                buf << s[i++];
            if (buf.str().length() < 1)
                throw ConfError(tmp, "Unrecognized parameter");
            route.setCodeRedirect(atoi(buf.str().c_str()));
            buf.clear();
            buf.str("");
            while (std::isspace(s[i]))
                i++;
            while (!std::isspace(s[i]) && s[i] != ';')
                buf << s[i++];
            route.setPathRedirect(buf.str());
        }
        else
            throw ConfError(tmp, "Unrecognized parameter");
        buf.clear();
        buf.str("");
        if (s[i] == ';')
            i++;
        else if (std::isspace(s[i]))
        {
            while (std::isspace(s[i]))
                i++;
            if (s[i] == ';')
                i++;
            else
                throw ConfError(std::string(1, s[i]), "Semicolon missing");
        }
        else
            throw ConfError(std::string(1, s[i]), "Semicolon missing");
    }
    return (route);
}

http::Routes http::Conf::check_inheritance(http::Routes &new_route, std::vector<http::Routes> all_routes, int i_server)
{
    std::vector<http::Routes>::iterator father_location;
    size_t                              len = 0;
    size_t                              max_len = 0;
    std::string                         path;

    for (std::vector<http::Routes>::iterator it = all_routes.begin(); it != all_routes.end(); it++)
    {
        path = it->getVirtualLocation();
        len = path.length();
        if (new_route.getVirtualLocation() == path || it->getOptModifier() == "=")
            continue ;
        if (new_route.getVirtualLocation().compare(0, len, path) == 0)
        {
            if (!max_len)
            {
                max_len = len;
                father_location = it;
            }
            else if (len > max_len)
            {
                max_len = len;
                father_location = it;
            }
        }
    }
    if (max_len)
    {
        // inheritance of WWW-Authentication
        if (father_location->needAuth() && !new_route.needAuth())
        {
            new_route.allowAuth();
            new_route.setAuthMessage(father_location->getAuthMessage());
            new_route.setPassAuthFile(father_location->getPassAuthFile());
        }
    }
    if (new_route.getDirPath() == "" && this->_servers[i_server].getRootDir() != "")
        new_route.setDirPath(this->_servers[i_server].getRootDir());
    return (new_route);
}

std::vector<http::ServerConf> http::Conf::getServers(void)
{
    return (this->_servers);
}

std::map<std::string, std::string> http::Conf::get_mime_types(void)
{
    return (this->_mime_types);
}

http::Conf::ConfError::ConfError(void)
{
	this->_error = "Exception in class Conf: Some error in Configuration File.";
}

http::Conf::ConfError::ConfError(std::string error)
{
	this->_error = "Exception in class Conf: " + error + ".";
}

http::Conf::ConfError::ConfError(std::string param, std::string error)
{
	this->_error = "Exception in class Conf: " + param + " \"" + error + "\".";
}

const char			*http::Conf::ConfError::what(void) const throw()
{
	return (this->_error.c_str());
}

#ifndef ROUTES_HPP
# define ROUTES_HPP

# include <string>
# include <iostream>
# include "../utils/params.hpp"

namespace http
{

class Routes
{

private:
    typedef struct  s_httpMethods
    {
        bool _GET;
        bool _POST;
        bool _HEAD;
        bool _PUT;
        bool _DELETE;
        bool _CONNECT;
        bool _OPTIONS;
        bool _TRACE;
        bool _PATCH;
    }               t_httpMethods;

    bool            _is_prefix;

    std::string     _extension;

    std::string     _optional_modifier;

    //Virtual location of the directory
    std::string     _location;

    // Limit client body size
    unsigned int    _body_size;

    // Path of a existing directory
    std::string     _directory_path;

    //HTTP Methods in a specific location
    t_httpMethods   _httpMethods;

    // Enable or disable directory listing. Default disable
    bool            _autoindex;

    // Default file to answer if the request is a directory
    std::vector<std::string>     _index_file;

    bool            _support_languages;
    // Suported languages in a location
    std::vector<std::string>     _languages;

    // CGI Parameters
    bool            _is_cgi;

    std::string     _cgi_exec;

    bool            _is_redirect;

    int             _code_redirect;

    std::string     _path_to_redirect;

    // Make the route able to accept uploaded files. Default false
    bool            _uploads;

    // Configure where uploads should be saved
    std::string     _upload_path;

    //Authentication
    bool            _is_auth;

    //Authentication off
    bool            _explicit_no_auth;

    //Message to Authenticate
    std::string     _auth_message;

    //Route of the password file Authentication
    std::string     _path_auth;

public:
    Routes()
    {
        this->_is_redirect = false;
        this->_code_redirect = 0;
        this->_path_to_redirect = std::string("");
        this->_support_languages = false;
        this->_is_prefix = false;
        this->_is_cgi = false;
        this->_cgi_exec = std::string("");
        this->_extension = std::string("");
        this->_optional_modifier = std::string("");
        this->_location = std::string("");
        this->_directory_path = std::string("");
        this->_body_size = DEFAULT_BODY_SIZE_BYTES;
        this->_httpMethods._CONNECT = false;
        this->_httpMethods._DELETE = false;
        this->_httpMethods._GET = false;
        this->_httpMethods._HEAD = false;
        this->_httpMethods._OPTIONS = false;
        this->_httpMethods._PATCH = false;
        this->_httpMethods._POST = false;
        this->_httpMethods._PUT = false;
        this->_httpMethods._TRACE = false;
        this->_autoindex = false;
        this->_uploads = false;
        this->_upload_path = std::string("");
        this->_is_auth = false;
        this->_explicit_no_auth = false;
        this->_auth_message = std::string("");
        this->_path_auth = std::string("");
    };

    Routes(const http::Routes &other)
    {
        this->_is_redirect = other._is_redirect;
        this->_code_redirect = other._code_redirect;
        this->_path_to_redirect = other._path_to_redirect;
        this->_support_languages = other._support_languages;
        this->_is_prefix = other._is_prefix;
        this->_is_cgi = other._is_cgi;
        this->_cgi_exec = other._cgi_exec;
        this->_extension = other._extension;
        this->_optional_modifier = other._optional_modifier;
        this->_location = other._location;
        this->_body_size = other._body_size;
        this->_directory_path = other._directory_path;
        this->_httpMethods._CONNECT = other._httpMethods._CONNECT;
        this->_httpMethods._DELETE = other._httpMethods._DELETE;
        this->_httpMethods._GET = other._httpMethods._GET;
        this->_httpMethods._HEAD = other._httpMethods._HEAD;
        this->_httpMethods._OPTIONS = other._httpMethods._OPTIONS;
        this->_httpMethods._PATCH = other._httpMethods._PATCH;
        this->_httpMethods._POST = other._httpMethods._POST;
        this->_httpMethods._PUT = other._httpMethods._PUT;
        this->_httpMethods._TRACE = other._httpMethods._TRACE;
        this->_autoindex = other._autoindex;
        this->_index_file = other._index_file;
        this->_languages = other._languages;
        this->_uploads = other._uploads;
        this->_upload_path = other._upload_path;
        this->_is_auth = other._is_auth;
        this->_explicit_no_auth = other._explicit_no_auth;
        this->_auth_message = other._auth_message;
        this->_path_auth = other._path_auth;
    };

    Routes      &operator=(const http::Routes &other)
    {
        this->_is_redirect = other._is_redirect;
        this->_code_redirect = other._code_redirect;
        this->_path_to_redirect = other._path_to_redirect;
        this->_support_languages = other._support_languages;
        this->_is_prefix = other._is_prefix;
        this->_is_cgi = other._is_cgi;
        this->_cgi_exec = other._cgi_exec;
        this->_extension = other._extension;
        this->_optional_modifier = other._optional_modifier;
        this->_location = other._location;
        this->_body_size = other._body_size;
        this->_directory_path = other._directory_path;
        this->_httpMethods._CONNECT = other._httpMethods._CONNECT;
        this->_httpMethods._DELETE = other._httpMethods._DELETE;
        this->_httpMethods._GET = other._httpMethods._GET;
        this->_httpMethods._HEAD = other._httpMethods._HEAD;
        this->_httpMethods._OPTIONS = other._httpMethods._OPTIONS;
        this->_httpMethods._PATCH = other._httpMethods._PATCH;
        this->_httpMethods._POST = other._httpMethods._POST;
        this->_httpMethods._PUT = other._httpMethods._PUT;
        this->_httpMethods._TRACE = other._httpMethods._TRACE;
        this->_autoindex = other._autoindex;
        this->_index_file = other._index_file;
        this->_languages = other._languages;
        this->_uploads = other._uploads;
        this->_upload_path = other._upload_path;
        this->_is_auth = other._is_auth;
        this->_explicit_no_auth = other._explicit_no_auth;
        this->_auth_message = other._auth_message;
        this->_path_auth = other._path_auth;
        return (*this);
    }

    virtual ~Routes() {};

    class ErrorOptionalMod: public std::exception {
        const char *what() const throw()
        {
            return ("File Configuration Exception: Optional Modifier Unrecognized");
        }
    };

    class ErrorLocation: public std::exception {
        const char *what() const throw()
        {
            return ("File Configuration Exception: Location Extension Unrecognized");
        }
    };


    void    setMethods(std::string mthd)
    {
        if (mthd == "GET")
            this->_httpMethods._GET = true;
        else if (mthd == "POST")
            this->_httpMethods._POST = true;
        else if (mthd == "HEAD")
            this->_httpMethods._HEAD = true;
        else if (mthd == "PUT")
            this->_httpMethods._PUT = true;
        else if (mthd == "DELETE")
            this->_httpMethods._DELETE = true;
        else if (mthd == "CONNECT")
            this->_httpMethods._CONNECT = true;
        else if (mthd == "OPTIONS")
            this->_httpMethods._OPTIONS = true;
        else if (mthd == "TRACE")
            this->_httpMethods._TRACE = true;
        else if (mthd == "PATCH")
            this->_httpMethods._PATCH = true;
    };

    bool MethodAllow(std::string method)
    {
        if (method == "GET")
            return (this->_httpMethods._GET);
        if (method == "POST")
            return (this->_httpMethods._POST);
        if (method == "HEAD")
            return (this->_httpMethods._HEAD);
        if (method == "PUT")
            return (this->_httpMethods._PUT);
        if (method == "DELETE")
            return (this->_httpMethods._DELETE);
        if (method == "CONNECT")
            return (this->_httpMethods._CONNECT);
        if (method == "OPTIONS")
            return (this->_httpMethods._OPTIONS);
        if (method == "TRACE")
            return (this->_httpMethods._TRACE);
        if (method == "PATCH")
            return (this->_httpMethods._PATCH);
        return (false);
    };

    bool MethodAllow(int method)
    {
        if (method == GET)
            return (this->_httpMethods._GET);
        if (method == POST)
            return (this->_httpMethods._POST);
        if (method == HEAD)
            return (this->_httpMethods._HEAD);
        if (method == PUT)
            return (this->_httpMethods._PUT);
        if (method == DELETE)
            return (this->_httpMethods._DELETE);
        if (method == CONNECT)
            return (this->_httpMethods._CONNECT);
        if (method == OPTIONS)
            return (this->_httpMethods._OPTIONS);
        if (method == TRACE)
            return (this->_httpMethods._TRACE);
        if (method == PATCH)
            return (this->_httpMethods._PATCH);
        return (false);
    };

    void        setLanguages()
    {
        this->_support_languages = true;
    };

    bool        supportLanguages()
    {
        return (this->_support_languages);
    };

    void        setPrefix()
    {
        this->_is_prefix = true;
    };

    bool        isPrefix()
    {
        return (this->_is_prefix);
    };

    void        setRedirect()
    {
        this->_is_redirect = true;
    };

    bool        isRedirect()
    {
        return (this->_is_redirect);
    };

    void        setCodeRedirect(int code)
    {
        this->_code_redirect = code;
    };

    int        getCodeRedirect()
    {
        return (this->_code_redirect);
    };

    void        setPathRedirect(std::string path)
    {
        this->_path_to_redirect = path;
    };

    std::string getPathRedirect()
    {
        return (this->_path_to_redirect);
    };

    void        setCgi()
    {
        this->_is_cgi = true;
    };

    bool        isCgi()
    {
        return (this->_is_cgi);
    };

    void        setCgiExec(std::string path_exec)
    {
        this->setCgi();
        this->_cgi_exec = path_exec;
    };

    std::string &getCgiExec()
    {
        return (this->_cgi_exec);
    };

    void        setBodySize(unsigned int new_body_size)
    {
        this->_body_size = new_body_size;
    };

    unsigned int &getBodySize(void)
    {
        return (this->_body_size);
    };

    unsigned long long getBodySizeinBytes(void)
    {
        return (this->_body_size * 1000000);
    };

    void        setExtension(std::string ext)
    {
        this->_extension = ext;
    };

    std::string &getExtension()
    {
        return (this->_extension);
    };

    void        setOptModifier(std::string opt)
    {
        if (opt != "~" && opt != "=" && opt != "~*" && opt == "^~")
            throw http::Routes::ErrorOptionalMod();
        this->_optional_modifier = opt;
    };

    std::string &getOptModifier()
    {
        return (this->_optional_modifier);
    };

    void        setLocation(std::string locat)
    {
        if (locat.front() == '/')
        {
            this->setPrefix();
            this->_location = locat;
        }
        else
        {
            if (locat.find(".") == std::string::npos)
                throw ErrorOptionalMod();
            this->_extension = http::trim2(locat, "\\$");
            this->_location = this->_extension;
            this->_extension = this->_extension.substr(locat.find("."));
        }
    };

    std::string &getVirtualLocation()
    {
        return (this->_location);
    };

    void        setDirPath(std::string dir_path)
    {
        this->_directory_path = dir_path;
    };

    std::string &getDirPath()
    {
        return (this->_directory_path);
    };

    void        allowAutoIndex()
    {
        this->_autoindex = true;
    };

    bool        getAutoIndex()
    {
        return (this->_autoindex);
    };

    void        addIndexFile(std::string index_file)
    {
        this->_index_file.push_back(index_file);
    };

    std::vector<std::string> getIndexFile()
    {
        return (this->_index_file);
    };

    void        addnewLanguage(std::string lang)
    {
        this->_languages.push_back(lang);
    };

    std::vector<std::string> getLanguages()
    {
        return (this->_languages);
    };

    void        allowUpload()
    {
        this->_uploads = true;
    };

    bool        getUpload()
    {
        return (this->_uploads);
    };

    void        setUploadPath(std::string up_path)
    {
        this->_upload_path = up_path;
    };

    std::string &getUploadPath()
    {
        return (this->_upload_path);
    };

    void        allowAuth()
    {
        this->_is_auth = true;
    };

    void        denyAuth()
    {
        this->_explicit_no_auth = true;
    };

    bool        needExplicitAuth()
    {
        return (this->_explicit_no_auth);
    };

    bool        needAuth()
    {
        if (this->_explicit_no_auth)
            return (false);
        return (this->_is_auth);
    };

    void        setAuthMessage(std::string auth_mess)
    {
        if (auth_mess == "off")
            this->denyAuth();
        else
        {
            this->allowAuth();
            this->_auth_message = trim2(auth_mess, "\"");
        }
    };

    std::string &getAuthMessage()
    {
        return (this->_auth_message);
    };

    void        setPassAuthFile(std::string auth_file)
    {
        this->_path_auth = auth_file;
    };

    std::string &getPassAuthFile()
    {
        return (this->_path_auth);
    };

    std::string getFileTransformed(std::string path_requested, std::vector<std::string> languages, int type, std::string &language_set)
    {
		std::string language_path = "";
		bool language_setted = false;

		if (this->_languages.size() > 0 && languages.size() > 0)
        {
			for (int i = 0; i < (int)languages.size() && !language_setted; i++)
            {
				for (int j = 0; j < (int)this->_languages.size() && !language_setted; j++)
                {
					if (this->_languages[j].find(languages[i]) != std::string::npos)
                    {
						language_path = this->_languages[j];
						language_setted = true;
					}
				}
			}
			if (!language_setted)
				language_path = this->_languages[0];
			language_set = language_path;
			language_path += "/";
		}
        if (!this->_is_prefix)
        {
            if (path_requested.front() == '/' && this->_directory_path.back() == '/')
                path_requested.erase(path_requested.begin());
            path_requested = this->_directory_path + path_requested;
        }
        else
        {
            std::string tmp = this->getDirPath();
            if (this->getVirtualLocation().back() == '/' && tmp.back() != '/')
                tmp += "/";
            path_requested.replace(0, this->getVirtualLocation().size(), tmp + language_path);
            if (type == POST && !this->_is_cgi)
                return (path_requested);
            if (path_requested.find(".") == std::string::npos && type != PUT)
            {
                for (std::vector<std::string>::iterator it = this->_index_file.begin(); it != this->_index_file.end(); it++)
                {
                    if (it->front() == '/' && path_requested.back() == '/')
                        path_requested.erase(path_requested.begin());
                    else if (path_requested.back() != '/' && it->front() != '/')
                        path_requested += '/';
                    path_requested += *it;
                    if (!http::file_exists(path_requested))
                    {
                        break ;
                    }
					path_requested = path_requested.substr(0, path_requested.length() - (*it).length());
                }
            }
        }
        if (path_requested.find(".") == std::string::npos && path_requested.back() != '/' && type != PUT)
            path_requested += '/';
        return (path_requested);
    };
};

inline std::ostream &operator<<(std::ostream &out, http::Routes &route)
{
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator itend;
    out << "\nLocation VirtualDirectory: " << route.getVirtualLocation() << std::endl;
    out << "Location isPrefix: " << ((route.isPrefix()) ? "on": "off") << std::endl;
    out << "Location isCGI: " << ((route.isCgi()) ? "on": "off") << std::endl;
    out << "Location CGI EXEC: " << route.getCgiExec() << std::endl;
    out << "Location Extension: " << route.getExtension() << std::endl;
    out << "Location BodySize: " << route.getBodySize() << std::endl;
    out << "Location isRedirect: " << ((route.isRedirect()) ? "on": "off") << std::endl;
    out << "Location Redirect Code: " << route.getCodeRedirect() << std::endl;
    out << "Location Redirect Path: " << route.getPathRedirect() << std::endl;
    out << "Location OptionalModifier: " << route.getOptModifier() << std::endl;
    out << "Location RootPath: " << route.getDirPath() << std::endl;
    out << "Location indexFile: ";
    it = route.getIndexFile().begin();
    itend = route.getIndexFile().end();
    for (; it != itend; it++)
        out << *it << " ";
    out << std::endl;
    out << "Location SupportLanguages: " << ((route.supportLanguages()) ? "on": "off") << std::endl;
    out << "Location SupportLanguages: ";
    it = route.getLanguages().begin();
    itend = route.getLanguages().end();
    for (; it != itend; it++)
        out << *it << " ";
    out << std::endl;
    out << "Location autoindex: " << ((route.getAutoIndex()) ? "on" : "off") << std::endl;
    out << "Location uploads: " << ((route.getUpload()) ? "on" : "off") << std::endl;
    out << "Location uploadsPath: " << route.getUploadPath() << std::endl;
    out << "Location authentication: " << ((route.needAuth()) ? "on" : "off") << std::endl;
    out << "Location AuthMessage: " << route.getAuthMessage() << std::endl;
    out << "Location Password Auth File: " << route.getPassAuthFile() << std::endl;
    out << "Location MethodsAllowed:" << std::endl;
    out << "\tGET: " << ((route.MethodAllow("GET")) ? "on": "off") << std::endl;
    out << "\tPOST: " << ((route.MethodAllow("POST")) ? "on": "off") << std::endl;
    out << "\tHEAD: " << ((route.MethodAllow("HEAD")) ? "on": "off") << std::endl;
    out << "\tDELETE: " << ((route.MethodAllow("DELETE")) ? "on": "off") << std::endl;
    out << "\tCONNECT: " << ((route.MethodAllow("CONNECT")) ? "on": "off") << std::endl;
    out << "\tOPTIONS: " << ((route.MethodAllow("OPTIONS")) ? "on": "off") << std::endl;
    out << "\tTRACE: " << ((route.MethodAllow("TRACE")) ? "on": "off") << std::endl;
    out << "\tPATCH: " << ((route.MethodAllow("PATCH")) ? "on": "off") << std::endl;
    out << "\tPUT: " << ((route.MethodAllow("PUT")) ? "on": "off") << std::endl;
    return (out);
};

} // namespace http

#endif

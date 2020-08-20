#ifndef ROUTES_HPP
# define ROUTES_HPP

# include <string>
# include <iostream>

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
    //Virtual location of the directory
    std::string     _location;

    // Path of a existing directory
    std::string     _directory_path;

    //HTTP Methods in a specific location
    t_httpMethods   _httpMethods;

    // Enable or disable directory listing. Default disable
    bool            _autoindex;

    // Default file to answer if the request is a directory
    std::string     _index_file;

    /*
        CGI parameters must also be implemented here
    */

    // Make the route able to accept uploaded files. Default false
    bool            _uploads;

    // Configure where uploads should be saved
    std::string     _upload_path;

public:
    Routes()
    {
        this->_location = std::string("");
        this->_directory_path = std::string("");
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
        this->_index_file = std::string("");
        this->_uploads = false;
        this->_upload_path = std::string("");
    };

    virtual ~Routes() {};

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

    void        setLocation(std::string locat)
    {
        this->_location = locat;
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

    void        setIndexFile(std::string index_file)
    {
        this->_index_file = index_file;
    };

    std::string &getIndexFile()
    {
        return (this->_index_file);
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
};

inline std::ostream &operator<<(std::ostream &out, http::Routes &route)
{
    out << "\nLocation VirtualDirectory: " << route.getVirtualLocation() << std::endl;
    out << "Location RootPath: " << route.getDirPath() << std::endl;
    out << "Location indexFile: " << route.getIndexFile() << std::endl;
    out << "Location autoindex: " << ((route.getAutoIndex()) ? "on" : "off") << std::endl;
    out << "Location uploads: " << ((route.getUpload()) ? "on" : "off") << std::endl;
    out << "Location uploadsPath: " << route.getUploadPath() << std::endl;
    out << "Location MethodsAllowed:" << std::endl;
    out << "\tGET: " << ((route.MethodAllow("GET")) ? "on": "off") << std::endl;
    out << "\tPOST: " << ((route.MethodAllow("POST")) ? "on": "off") << std::endl;
    out << "\tHEAD: " << ((route.MethodAllow("HEAD")) ? "on": "off") << std::endl;
    out << "\tDELETE: " << ((route.MethodAllow("DELETE")) ? "on": "off") << std::endl;
    out << "\tCONNECT: " << ((route.MethodAllow("CONNECT")) ? "on": "off") << std::endl;
    out << "\tOPTIONS: " << ((route.MethodAllow("OPTIONS")) ? "on": "off") << std::endl;
    out << "\tTRACE: " << ((route.MethodAllow("TRACE")) ? "on": "off") << std::endl;
    out << "\tPATCH: " << ((route.MethodAllow("PATCH")) ? "on": "off") << std::endl;
    return (out);
};

} // namespace http

#endif

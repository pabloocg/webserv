#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <string>
#include <iostream>

namespace http
{

class Routes
{

private:
    typedef struct s_httpMethods
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
    } t_httpMethods;
    //Virtual location of the directory
    std::string _location;

    // Path of a existing directory
    std::string _directory_path;

    //HTTP Methods in a specific location
    t_httpMethods _httpMethods;

    // Enable or disable directory listing. Default disable
    bool _autoindex;

    // Default file to answer if the request is a directory
    std::string _index_file;

    /*
        CGI parameters must also be implemented here
    */

    // Make the route able to accept uploaded files. Default false
    bool _uploads;

    // Configure where uploads should be saved
    std::string _upload_path;

public:
    Routes(/* args */);
    virtual ~Routes(){};

    bool    MethodAllow(std::string method)
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
    }

    std::string getVirtualLocation()
    {
        return (this->_location);
    };

    std::string getDirPath()
    {
        return (this->_directory_path);
    };

    bool getAutoIndex()
    {
        return (this->_autoindex);
    };

    std::string getIndexFile()
    {
        return (this->_index_file);
    };

    bool getUpload()
    {
        return (this->_uploads);
    };

    std::string getUploadPath()
    {
        return (this->_upload_path);
    };
};

} // namespace http

#endif

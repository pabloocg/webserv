#include "Request.hpp"
#include "utils.hpp"

void http::Request::save_header(std::string header){
	std::vector<std::string> words;
	words = http::split(header, ' ');

	if (words[0] == "Accept-Charsets:"){

	}
	else if (words[0] == "Accept-Language:"){

	}
	else if (words[0] == "Allow:"){

	}
	else if (words[0] == "Authorization:"){

	}
	else if (words[0] == "Content-Language:"){

	}
	else if (words[0] == "Content-Length:"){

	}
	else if (words[0] == "Content-Location:"){

	}
	else if (words[0] == "Content-Type:"){

	}
	else if (words[0] == "Date:"){

	}
	else if (words[0] == "Host:"){

	}
	else if (words[0] == "Location:"){

	}
	else if (words[0] == "Referer:"){

	}
	else if (words[0] == "Retry-After:"){

	}
	else if (words[0] == "Server:"){

	}
	else if (words[0] == "Transfer-Encoding:"){

	}
	else if (words[0] == "User-Agent:"){

	}
	else if (words[0] == "WWW-Authenticate:"){

	}

}

http::Request::Request(std::string req){
	std::cout << "************ REQUEST ************" << std::endl;
	std::cout << req << std::endl;
	std::cout << "*********************************" << std::endl;
	this->request = req;

	std::vector<std::string> sheader;
	std::vector<std::string> request;

	sheader = http::split(req, '\n');

	request = http::split(sheader[0], ' ');
	if (request[0] == "GET")
		this->type = GET;
	else if (request[0] == "HEAD")
		this->type = HEAD;
	else if (request[0] == "OPTIONS")
		this->type = OPTIONS;
	else if (request[0] == "POST")
		this->type = POST;
	else if (request[0] == "PUT")
		this->type = PUT;
	
	this->file_req = ROOT_DIR + request[1];
	if (this->type == GET && request[1] == "/")
		this->file_req = "dir/index.html";

	
	this->file_type = this->file_req.substr(file_req.find(".") + 1, file_req.size());
	if (this->file_req == this->file_type && this->type == GET)
		this->file_type = "html";
	this->http_version = request[2];

	for (int i = 1; i < (int)sheader.size(); i++){
		this->save_header(sheader[i]); //esto leera cada header y lo guardara en su respectiva variable privada de Request
	}
	
}

std::string http::Request::get_content_type(std::string type){
	if (type == "htm")
		type = "html";
	else if (type == "ics")
		type = "calendar";
	if (type == "css" || type == "csv" || type == "html" || type == "calendar" || type == "csv-scherma" || type == "xml")
		return("text/" + type);
	if (type == "jpg")
		return("image/jpeg");
	if (type == "ico")
		return("image/x-icon");
	if (type == "woff" || type == "woff2" || type == "ttf")
		return ("font/" + type);
	else
		return ("text/plain");
}

char *http::Request::build_get(){
	char *res;
	std::ifstream file;
	std::string buf;
	std::stringstream stream;

	file.open(this->file_req);
	if (file.is_open())
	{
		this->status = 200;
		this->message_status = "OK";
	}
	//Read the error page 404
	else
	{
		file.open("dir/404.html");
		this->status = 404;
		this->message_status = "Not Found";
	}

	while (std::getline(file, buf))
		stream << buf << "\n";
	this->resp_body = stream.str();
	stream.str("");
	stream.clear();
	file.close();

	stream << "HTTP/1.1 " << this->status << " " << this->message_status << "\nContent-Type: "
	<< get_content_type(this->file_type) << "\nContent-Length: " << this->resp_body.length() << "\n\n" << this->resp_body;

	this->resp_body = stream.str();
	std::cout << "************ RESPONSE ***********" << std::endl;
	std::cout << this->resp_body << std::endl;
	std::cout << "*********************************" << std::endl;
	if (!(res = (char *)malloc(sizeof(char) * (this->resp_body.size() + 1))))
		return (NULL);
	std::copy(this->resp_body.begin(), this->resp_body.end(), res);
	res[this->resp_body.size()] = '\0';
	return (res);
}

char *http::Request::build_response(){
	if (this->type == GET){
		return (this->build_get());
	}
	return (NULL);
}




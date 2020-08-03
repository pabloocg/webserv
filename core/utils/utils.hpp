#ifndef UTILS_HPP
#define UTILS_HPP

# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <algorithm>
# include <regex>

#define ROOT_DIR "dir"

namespace http
{

static std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string tok;

	while (std::getline(ss, tok, delimiter))
		internal.push_back(tok);
	return (internal);
}

static std::string		ltrim(std::string str)
{
	return (std::regex_replace(str, std::regex("^\\s+"), std::string("")));
}

static std::string		rtrim(std::string str)
{
	return (std::regex_replace(str, std::regex("\\s+$"), std::string("")));
}

inline std::string		trim(std::string str)
{
	return (rtrim(ltrim(str)));
}


//Esta funcion tendrá que ir en la clase Request
static char *read_file(std::string file_request)
{
	std::ifstream file;
	std::stringstream content_file;
	std::string buf;
	char *res;
	int status;
	std::string message_status;
	std::string file_type;

	//Read the file requested
	file_type = file_request.substr(file_request.find(".") + 1, file_request.size());
	if (file_request == file_type)
		file_type = "html";
	std::cout << file_type << std::endl;
	std::cout << "******" << file_request << "*******" << std::endl;
	if (file_request == "dir/")
		file_request = "dir/index.html";
	file.open(file_request);
	if (file.is_open())
	{
		status = 200;
		message_status = "OK";
	}
	//Read the error page 404
	else
	{
		file.open("dir/404.html");
		status = 404;
		message_status = "Not Found";
	}
	//Should also do one if the client not have permissions to read the file page 403
	while (std::getline(file, buf))
		content_file << buf << "\n";
	buf = content_file.str();
	content_file.str("");
	content_file.clear();
	file.close();
	content_file << "HTTP/1.1 " << status << " " << message_status << "\nContent-Type: text/" << file_type << "\ncharset=utf-8\nContent-Length: " << buf.length() << "\n\n"
				 << buf;
	buf = content_file.str();
	if (!(res = (char *)malloc(sizeof(char) * (buf.size() + 1))))
		return (NULL);
	std::copy(buf.begin(), buf.end(), res);
	res[buf.size()] = '\0';
	return (res);
}

inline char *parse_headers(std::string header)
{
	std::vector<std::string> sheader;
	std::vector<std::string> request;

	sheader = split(header, '\n');
	request = split(sheader[0], ' ');
	if (request[0] == "GET")
		return (read_file(ROOT_DIR + request[1]));
	return (NULL);
}
} // namespace http

#endif

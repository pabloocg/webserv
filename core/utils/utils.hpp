#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <regex>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define ROOT_DIR "dir"

namespace http
{

	inline int	file_exists(std::string file)
	{
		struct stat	buff;

		return (stat(file.c_str(), &buff));
	}

	inline std::vector<std::string> special_split(std::string str, char delimiter)
	{
		std::vector<std::string> internal;
		std::stringstream ss;

		for (size_t i = 0; i < str.length(); i++)
		{
			if (str[i] == delimiter)
			{
				internal.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else if (str[i] == '{')
			{
				while (str[i] != '}')
					ss << str[i++];
				ss << str[i];
				internal.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else
				ss << str[i];
		}
		return (internal);
	}

	inline std::vector<std::string> split(std::string str, char delimiter)
	{
		std::vector<std::string> internal;
		std::stringstream ss(str);
		std::string tok;

		while (std::getline(ss, tok, delimiter))
		{
			if (tok.size() > 0)
				internal.push_back(tok);
		}
		return (internal);
	}

	static std::string ltrim(std::string str)
	{
		return (std::regex_replace(str, std::regex("^\\s+"), std::string("")));
	}

	static std::string rtrim(std::string str)
	{
		return (std::regex_replace(str, std::regex("\\s+$"), std::string("")));
	}

	inline std::string trim(std::string str)
	{
		return (rtrim(ltrim(str)));
	}

	inline std::string trim2(std::string s, std::string trimChars)
	{
		std::stringstream	buff;
		size_t				i = 0;
		while (i < s.length())
		{
			if (trimChars.find(s[i]) == std::string::npos)
				buff << s[i];
			i++;
		}
		return (buff.str());
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

		std::cout << header << std::endl;

		sheader = split(header, '\n');
		request = split(sheader[0], ' ');
		if (request[0] == "GET")
			return (read_file(ROOT_DIR + request[1]));
		return (NULL);
	}

	inline int sendall(int s, char *buf, int *len)
	{
		int total = 0;		  
		int bytesleft = *len;
		int n;
		int sended = 0;
		while (total < *len)
		{
			if (sended){
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(s, &writefds);
				select(s + 1, NULL, &writefds, NULL, NULL);
			}
			n = send(s, buf + total, bytesleft, 0);
			if (n == -1)
			{
				break;
			}
			total += n;
			bytesleft -= n;
			sended = 1;
			std::cout << "sended " << n << " bytes, " << total << "/" << *len << ", left:" << bytesleft << std::endl;
		}
		*len = total;			 
		return n == -1 ? -1 : 0;
	}

	typedef unsigned char uchar;
	static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
	inline std::string base64_decode(const std::string &in) {

    std::string out;

    std::vector<int> T(256,-1);
    for (int i=0; i<64; i++) T[b[i]] = i;

    int val=0, valb=-8;
    for (int i = 0; i < (int)in.size(); i++) {
		uchar c = (uchar)in[i];
        if (T[c] == -1) break;
        val = (val<<6) + T[c];
        valb += 6;
        if (valb>=0) {
            out.push_back(char((val>>valb)&0xFF));
            valb-=8;
        }
    }
    return out;
}

} // namespace http

#endif

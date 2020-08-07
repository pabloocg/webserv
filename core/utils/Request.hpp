#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <algorithm>

#define GET 0
#define HEAD 1
#define OPTIONS 2
#define POST 3
#define PUT 4


namespace http
{
	class Request
	{
	private:

	//Request variables
	int type;
	std::string file_req;
	std::string file_type;
	std::string request;
	std::string http_version;

	//Response variables
	int status;
	std::string message_status;
	std::string resp_body;

	char *build_get(int *size);
	std::string get_content_type(std::string file_type);

	public:
		Request(std::string req);

		char *build_response(int *size);
		void save_header(std::string header);
	};
} // namespace http

#endif
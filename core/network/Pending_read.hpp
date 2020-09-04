#ifndef PENDING_READ_HPP
#define PENDING_READ_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <iostream>
# include <string>
# include <fcntl.h>
# include "../config/Logger.hpp"
# include "../request/Request.hpp"
# include "../utils/utils.hpp"
# include "../config/ServerConf.hpp"
# include "../utils/Pending_send.hpp"


namespace http
{
	class Pending_read
	{
	public:
		std::string message;
		std::string headers;
		bool isChunked;
		bool isLength;
		bool headers_read;
		bool badRequest;
		std::string host_header;
		int bodyLength;

		Pending_read(void);

		void operator=(Pending_read const &other);
		bool is_valid_format(void);
	};
} // namespace http

#endif
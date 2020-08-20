#ifndef PENDING_SEND_HPP
#define PENDING_SEND_HPP

#include <stdio.h>
#include <string>


namespace http
{
	class Pending_send
	{
	private:
		char *message;
		int size;
		int sended;
		int left;
	public:
		Pending_send(char *message, int size, int sended, int left);
		Pending_send(void);

		void operator=(Pending_send const &other);

		char *get_message(void);
		int get_size(void);
		int get_sended(void);
		int get_left(void);
		void set_sended(int sended);
		void set_left(int sended);
	};
} // namespace http

#endif
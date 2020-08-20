#include "Pending_send.hpp"

http::Pending_send::Pending_send(char *message, int size, int sended, int left):
message(message), size(size), sended(sended), left(left)
{
}

http::Pending_send::Pending_send(void){
	message = NULL;
	size = 0;
	sended = 0;
	left = 0;
}

void http::Pending_send::operator=(http::Pending_send const &other){
	this->message = other.message;
	this->size = other.size;
	this->sended = other.sended;
	this->left = other.left;
}

char *http::Pending_send::get_message(void){
	return (this->message);
}

int http::Pending_send::get_size(void){
	return (this->size);
}

int http::Pending_send::get_sended(void){
	return (this->sended);
}

int http::Pending_send::get_left(void){
	return (this->left);
}

void http::Pending_send::set_sended(int sended){
	this->sended = sended;
}

void http::Pending_send::set_left(int left){
	this->left = left;
}
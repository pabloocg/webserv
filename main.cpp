#include "Server.hpp"

int main(){
	http::Server serv;
	serv.start();
	while(1){
		serv.wait_for_connection();
	}
	return 0;
}

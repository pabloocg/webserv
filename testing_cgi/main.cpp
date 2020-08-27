#include<iostream>
#include<fstream>
#include<unistd.h>
#include <random>

int main(int argc, char **argv, char **env){
	char buffer[3000] = {};
	int valread;
	std::cout << "me llegan las siguientes variables de entorno:\n";
	for (int i = 0; env[i]; i++){
		std::cout << env[i] << std::endl;
	}
	valread = read(0, buffer, 3000);
	if (valread > 0)
		std::cout << "y por la stdin:\n" << buffer << std::endl;
	else
		std::cout << "y por la stdin NADA \n";
	
}
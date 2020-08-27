#include<iostream>
#include<fstream>
#include<unistd.h>
#include <random>
#include "../core/utils/base64.hpp"

/*====== MAIN ======*/
int main(int argc, char** argv) {

    std::string username, password, crypted;

    std::cout << "Enter your username: ";
    if (! std::getline(std::cin, username)) return 1;
    std::cout << "Enter your password: ";
    if (! std::getline(std::cin, password)) return 1;

	std::vector<unsigned char> vec;

	for(int i = 0; i < (int)password.length(); i++){
		vec.push_back((unsigned char)password[i]);
	}

    crypted = username + ":" + base64::encode(vec);

    std::cout << "New password generated:\n" << crypted << "\nPaste it into your .htpasswd file" << std::endl;
    return 0;
}
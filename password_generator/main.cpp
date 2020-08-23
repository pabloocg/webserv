#include<iostream>
#include<fstream>
#include<unistd.h>
#include <random>


using namespace std;

std::string generateSalt() {

    const char alphanum[] =
            "0123456789"
            "!@#$%^&*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"; //salt alphanum

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, sizeof(alphanum)-1); //Uniform distribution on an interval
    char salt[22];          // 21 useful characters in salt (as in original code)
    for(char& c: salt) {
        c = alphanum[dis(gen)];
    }
    salt[21] = 0;
    return std::string(salt);
}

/*====== MAIN ======*/
int main(int argc, char** argv) {

    string username, password, salt, hash;

    cout << "Enter your username: ";
    if (! getline(cin, username)) return 1;
    cout << "Enter your password: ";
    if (! getline(cin, password)) return 1;
	salt = generateSalt();

    hash = crypt(password.c_str(), "Y");

    std::cout << username << ":" << hash << ", with salt:" << salt;
    return 0;
}
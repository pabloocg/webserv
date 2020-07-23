// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

#define PORT 8080

#define ROOT_DIR "dir"

std::vector<std::string> split(std::string str, char delimiter)
{
  std::vector<std::string>  internal;
  std::stringstream         ss(str);
  std::string               tok;

  while(std::getline(ss, tok, delimiter))
    internal.push_back(tok);
  return (internal);
}

char        *read_file(std::string file_request)
{
    std::ifstream       file;
    std::stringstream   content_file;
    std::string         buf;
    char                *res;
    int                 status;
    std::string         message_status;
    std::string         file_type;

    //Read the file requested
    //file_request = file_request.substr(1, file_request.size());
    file_type = file_request.substr(file_request.find(".") + 1, file_request.size());
    if (file_request == file_type)
        file_type = "html";
    std::cout << file_type << std::endl;
    std::cout << "******" << file_request << "*******" << std::endl;
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
    content_file << "HTTP/1.1 " << status << " " << message_status <<
    "\nContent-Type: text/" << file_type <<"\ncharset=utf-8\nContent-Length: " << buf.length() << "\n\n" << buf;
    buf = content_file.str();
    if (!(res = (char*)malloc(sizeof(char) * (buf.size() + 1))))
        return (NULL);
    std::copy(buf.begin(), buf.end(), res);
    res[buf.size()] = '\0';
    return (res);
}

char        *parse_headers(std::string header)
{
    std::vector<std::string> sheader;
    std::vector<std::string> request;

    sheader = split(header, '\n');
    request = split(sheader[0], ' ');
    if (request[0] == "GET")
        return (read_file(ROOT_DIR + request[1]));
    return (NULL);
}


int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char    *res;
    
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        res = parse_headers(buffer);
        if (!res)
        {
            perror("some error occured");
            exit(EXIT_FAILURE);
        }
        write(new_socket , res , strlen(res));
        free(res);
        printf("------------------Requested file sent-------------------");
        close(new_socket);
    }
    free(res);
    return 0;
}

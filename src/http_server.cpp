#include "http_server.h"
#include <unistd.h> // read()
#include <cstring> // strncmp
#include <iostream>
#include <sstream> //istringstream

HttpServer::HttpServer(std::string port): TcpServer(port) {}

void HttpServer::processRequest(int client_fd) {
    char buffer[1024] = {0};
    int valread = read(client_fd, buffer, 1024);
    buffer[valread] = '\0';
    if(!isHTTP(buffer)){
        std::cerr<<"Server: Can only handle HTTP requests!";
        exit(1);
    }
    struct HttpRequest parsed_request = parseRequest(buffer);
    std::string resp;
    if(parsed_request.method == "GET" && parsed_request.path == "/"){
        resp = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";

    }
    else if(parsed_request.method == "GET" && parsed_request.path == "/index.html"){
        resp = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 23\r\n"
            "\r\n"
            "Hello, world from home!";
    }
    else{
        resp = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
    send(client_fd, resp.c_str(), resp.size(), 0);
    return;
}

HttpRequest HttpServer::parseRequest(char* request){
    std::vector<std::string>parsed_request;
    int start = 0;
    for(int i=0; i<strlen(request); i++){
        if(i+1 < strlen(request) && request[i] == '\r' && request[i+1] == '\n'){
            std::string request_part = "";
            for(int j=start; j<i; j++){
                request_part += request[j];
            }
            parsed_request.push_back(request_part);
            start = i+2;
        }
    }
    // parse request line which contains method, path, version
    std::istringstream iss(parsed_request[0]);
    std::string s;
    int counter = 0;
    struct HttpRequest http_request;
    while(getline(iss, s, ' ')){
        if(counter == 0){
            http_request.method = s;
        }
        else if(counter == 1){
            http_request.path = s;
        }
        counter++;
    }
    return http_request;
}
bool HttpServer::isHTTP(char *request){
    if (strncmp(request, "GET ", 4) == 0 || strncmp(request, "POST ", 5) == 0 || 
        strncmp(request, "HEAD ", 5) == 0 || strncmp(request, "PUT ", 4) == 0 || 
        strncmp(request, "DELETE ", 7) == 0 || strncmp(request, "OPTIONS ", 8) == 0 || 
        strncmp(request, "CONNECT ", 8) == 0 || strncmp(request, "TRACE ", 6) == 0) {
        std::cout << "Received an HTTP request:" << std::endl;
        std::cout << request << std::endl;
        return true;
    } else {
        std::cout << "Received a non-HTTP request." << std::endl;
        return false;
    }
}

#include "tcp_server.h"
#include <vector>

struct HttpRequest {
    std::string method;
    std::string path;
    std::vector<std::string> headers;
    std::string body;
};

class HttpServer : public TcpServer {
    public:
        HttpServer(std::string port);
    private:
        void processRequest(int client_fd);
        bool isHTTP(char* request);
        HttpRequest parseRequest(char* request);
};
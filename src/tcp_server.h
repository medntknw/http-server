#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> // struct addrinfo
#include <string>


class TcpServer{
    public:
        TcpServer(std::string port);
        ~TcpServer();
        void run();
    protected:
        int sendall(int client_fd, const char* buffer, int *len);
    private:
        std::string m_port;
        int queue_size;
        int m_socket;

        int startServer();
        void handleConnections();
        void closeServer();
        virtual void processRequest(int client_fd) = 0;
        void *getInAddr(struct sockaddr *sa);
};
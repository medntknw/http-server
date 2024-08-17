#include "tcp_server.h"
#include <cstring> // memset
#include <iostream> // cin, cout, cerr
#include <unistd.h> // used for close()



TcpServer::TcpServer(std::string port){
    m_port = port;
    queue_size = 10;
}
TcpServer::~TcpServer(){
    std::cout<<"Closing all connections...";
    closeServer();
}
void TcpServer::run(){
    if(startServer() != 0){
        std::cerr<<"Server: Failed to start server!\n";
        exit(1);
    }
    handleConnections();
}

int TcpServer::sendall(int client_fd, const char* buffer, int * len){
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(client_fd, buffer+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}
int TcpServer::startServer(){
    int yes=1;
    struct addrinfo hints; // information with the request basically flags etc
    struct addrinfo *res, *p; // stores the actual response from the DNS
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill in my ip
    getaddrinfo(NULL, m_port.c_str(), &hints, &res);

    // create socket -> bind -> listen
    // traversing the response for the addrinfo
    for(p = res; p != NULL; p = p->ai_next){
        m_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(m_socket < 0){
            std::cerr<<"Server: Cannot create Socket\n";
            continue;
        }
        /*
        This option allows a socket to bind to an address that is already in use. 
        Specifically, it tells the kernel to allow reuse of local addresses in bind() calls. 
        This is useful for restarting a server immediately after it has been closed,
        without waiting for the operating system to release the port.
        */
        if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            std::cerr<<"Server: Failed to update socket configuration\n";
            return 1;
        }
        char ip4[INET_ADDRSTRLEN];
        inet_ntop(p->ai_family, p->ai_addr, ip4, p->ai_addrlen);
        std::cout<<"Server: IPv4 "<<ip4<<'\n';

        if(bind(m_socket, p->ai_addr, p->ai_addrlen) == -1){
            close(m_socket);
            std::cerr<<"Server: Failed to bind to the socket\n";
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if(p == NULL){
        std::cerr<<"Server: Failed to bind \n";
        return 1;
    }
    if(listen(m_socket, queue_size) == -1){
        std::cerr<<"Server: Cannot listen to the socket: "<<m_socket<<'\n';
        return 1;
    }

    std::cout<<"Server: Waiting for connections...\n";
    return 0;
    
}
void TcpServer::handleConnections(){
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int client_fd;
    char s[INET6_ADDRSTRLEN];
    while(1) {
        sin_size = sizeof client_addr;
        client_fd = accept(m_socket, (struct sockaddr *)&client_addr, &sin_size);
        if (client_fd == -1) {
            std::cerr<<"Server: Error accepting client\n";
            continue;
        }

        inet_ntop(client_addr.ss_family, getInAddr((struct sockaddr *)&client_addr), s, sizeof s);
        printf("Server: Got connection from %s\n", s);

        processRequest(client_fd);

        // if (!fork()) { // this is the child process
        //     close(m_socket); // child doesn't need the listener
        //     if (send(client_fd, "Hello, world!", 13, 0) == -1)
        //         perror("send");
        //     close(client_fd);
        //     exit(0);
        // }
        close(client_fd);  // parent doesn't need this
    }

}

void *TcpServer::getInAddr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void TcpServer::closeServer(){
    std::cout<<"Closing socket: "<<m_socket<<'\n';
    close(m_socket);
}
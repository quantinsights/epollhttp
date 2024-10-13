#ifndef tcp_server_H
#define tcp_server_H

#include <string>
#include <memory>

class TCPServer{
    public:
        virtual bool initialize(int port, const std::string& ip_address = "127.0.0.1") = 0;
        virtual void start() = 0; // Start the server and listen for connections
        virtual ~TCPServer() = default;

        static std::shared_ptr<TCPServer> buildTCPServer();
};


#endif
#ifndef linux_server_H
#define linux_server_H

#include <stdio.h>
#include <iostream>         
#include <stdlib.h>         // For exit()
#include <string.h>         // For memset
#include <unistd.h>         // For close()
#include <arpa/inet.h>      // For socket functions and structures
#include <sys/socket.h>
#include <fstream>          // For file I/O
#include <thread>
#include "../include/tcpserver.h"
#include "../include/ctpl_stl.h"

class LinuxServer : public TCPServer
{
    private:
        int m_server_socket;
        struct sockaddr_in m_address;     // Structure to hold server's address information
        socklen_t m_addrlen = sizeof(m_address);
        std::thread m_epoll_thread;
        int m_epoll_fd;
        void epollLoop();
    public:
        LinuxServer();
        virtual ~LinuxServer();

        bool initialize(int port, const std::string& ip_address = "127.0.0.1");
        void start(); // Start the server and listen for connections
        static std::shared_ptr<LinuxServer> buildTCPServer();
};

#endif
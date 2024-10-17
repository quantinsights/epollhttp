#ifndef linux_server_H
#define linux_server_H

#include <stdio.h>
#include <iostream>         
#include <stdlib.h>         // For exit()
#include <string.h>         // For memset
#include <unistd.h>         // For close()
#include <arpa/inet.h>      // For socket functions and structures
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <thread>
#include <unordered_map>
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
        bool setSocketAsNonBlocking(int socked_id);
        ctpl::thread_pool m_thread_pool;

        //Use a map to store the state of each client
        std::unordered_map<int, ClientState> m_client_state_map;

        void handleRecv(int client_socket);
        void handleSend(int client_socket);
        void handleError(int client_socket);
    public:
        LinuxServer();
        virtual ~LinuxServer();

        bool initialize(int port, const std::string& ip_address = "127.0.0.1");
        void start(); // Start the server and listen for connections
        static std::shared_ptr<LinuxServer> buildTCPServer();
};

#endif
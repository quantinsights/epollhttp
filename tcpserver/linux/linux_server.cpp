#include "linux_server.hpp"

LinuxServer::LinuxServer() : m_server_socket{-1} {}

bool LinuxServer::initialize(int port, const std::string& ip_address)
{   

    std::cout << "\nInitializing the server instance.";
    // 1. Create a TCP Socket using IPv4 protocol (AF_INET) and stream type SOCK_STREAM
    if((m_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cout << "Failed to create socket. errno : " << errno << std::endl;
        return false;
    }

    std::cout << "\nCreated a new server socket";

    // 2. Configure the server address
    // Set address family to IpV4 (AF_INET)
    m_address.sin_family = AF_INET;

    // Listen to all available network interfaces and specify the port number
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(8080);     //Convert port to network byte order (big-endian)

    // Set socket options to reuse the address and port
    const int enable = 1;
    if(setsockopt(m_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) < 0)
    {
        std::cerr << "\nsetsockopt(SO_REUSEADDR) failed";
        close(m_server_socket);
        return false;
    }

    // 3. Bind the socket to IP address and port
    if(bind(m_server_socket, (struct sockaddr*) &m_address, m_addrlen) < 0)
    {
        std::cout << "\nFailed to bind to specified address and port. errno : " << errno << std::endl;
        close(m_server_socket);
        return false;
    }

    std::cout << "\nBinding the socket to specified address and port completed.";

    // 4. Listen for incoming connections
    // The second argument defines the backlog (number of pending connections allowed)
    const int backlog = 10000;
    if(listen(m_server_socket, backlog) < 0)
    {
        perror("\nListen failed...");
        close(m_server_socket);
        return false;
    }

    std::cout << "\nListening for incoming connections...";
    return true;
}

void LinuxServer::start()
{
    // This while loop will handle multiple client connections. accept() is a blocking
    // call. The accept() function shall extract the first connection on the circular queue of pending connections, 
    // create a new socket with the same socket type protocol and address family as the specified socket, 
    // and allocate a new file descriptor for that socket. 

    // We are going to create a separate thread for the epoll loop. epoll_wait() is a blocking call. So,
    // our server thread should will not become busy on epoll_wait(), and is free to accept new connections from
    // the connection-pending queue.

    m_epoll_fd = epoll_create1(0);
    if(m_epoll_fd == -1)
    {
        std::cout << "\nFailed to create epoll. errno : " << errno;
        return;
    }
    
    while(true)
    {
        // 5. Accept a connection from the client
        std::cout << "\nWaiting to accept a connection..." << std::endl;
        int client_socket = accept(m_server_socket, (struct sockaddr*) &m_address, (socklen_t*) &m_addrlen);
        if(client_socket < 0)
        {
            std::cout << "Failed to accept connection from the client. errno : " << errno << std::endl;
            continue;
        }

        std::cout << "\nConnection accepted.";

        // Submit the client handling task to the thread pool.
        //pool.push(handle_client, client_socket);
    }
}

void LinuxServer::epollLoop()
{

}

static std::shared_ptr<LinuxServer> buildTCPServer()
{

}
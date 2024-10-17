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

bool LinuxServer::setSocketAsNonBlocking(int client_socket)
{
    int flags = fcntl(client_socket, F_GETFL, 0);

    if(flags == -1)
    {
        std::cout << "\nError getting socket flags." << std::endl;
        return false;
    }

    flags |= O_NONBLOCK;
    if(fcntl(client_socket, F_SETFL, 0) == -1)
    {
        std::cout << "\nError setting socket flags to O_NONBLOCK.";
        return false;
    }
    std::cout << "\nSet socket id = " << client_socket << " to non-blocking mode.";
    return true;
}

void LinuxServer::start()
{
    // This while loop will handle multiple client connections. accept() is a blocking
    // call. The accept() function shall extract the first connection on the circular queue of pending connections, 
    // create a new socket with the same socket type protocol and address family as the specified socket, 
    // and allocate a new file descriptor for that socket. 

    // This will return an epoll instance. We can think of this as a monitor object(a data-structure that the OS keeps)
    // that maintains the monitoring list.  

    m_epoll_fd = epoll_create1(0);
    if(m_epoll_fd == -1)
    {
        std::cout << "\nFailed to create epoll. errno : " << errno;
        return;
    }

    // accept() is a blocking call. The work of polling FDs and returning those
    // which are read/write ready doesn't have to wait on accept(). It can be done together concurrently 
    // with accept()'ing connections off the pending queue.
    // So, we spawn a new thread to poll FDs.
    m_epoll_thread = std::thread(&epollLoop, this);
    m_epoll_thread.detach();

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

        if(!setSocketAsNonBlocking(client_socket))
        {
            std::cout << "\nClosing client socket " << client_socket;
            close(client_socket);
            continue;
        }

        // An epoll_ctl() will allow us to add, modify and delete file-descriptors from the epoll instance.
        // When you add a file descriptor, you store struct epoll_event instance that later you receive 
        // when the file descriptor has something new to process.

        // Level-triggered(LT) epoll will keep notifying you as long as the FD remains read or write ready, whereas
        // with edge-triggered(ET), only one notification is set for the FD, when the data is there. 

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.fd = client_socket;
        epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, client_socket, &ev);

    }
}

/*
* Keep polling FDs to see which ones are read/write-ready
*/
void LinuxServer::epollLoop()
{
    const int max_events = 8;
    struct epoll_event events[max_events];

    while(true)
    {
        // The events parameter is a buffer that will contain triggered events.
        int event_count = epoll_wait(m_epoll_fd, events, max_events, -1);   //Blocking call
        for(int i{0}; i < event_count; ++i)
        {
            int client_socket = events[i].data.fd;
            
            if(events[i].events & EPOLLIN)
            {
                m_thread_pool.push([this, client_socket](int thread_id){
                    this->handleRecv(client_socket);
                });
            }
            else if(events[i].events & EPOLLOUT)
            {
                m_thread_pool.push([this, client_socket](int thread_id){
                    this->handleSend(client_socket);
                });
            }
            else if(events[i].events & EPOLLERR)
            {
                m_thread_pool.push([this, client_socket](int thread_id){
                    this->handleError(client_socket);
                });
            }
        }
    }
}

void handleSend(int client_socket)
{

}

void LinuxServer::handleRecv(int client_socket)
{
    if(m_client_state_map.find(client_socket) == m_client_state_map.end())
    {
        m_client_state_map[client_socket] = ClientState(client_socket);
    }
    const int buffer_size = 1024;
    char recv_buffer[1024];
    std::string data;

    int bytes_received = recv(client_socket, recv_buffer, 1024, 0);

    if(bytes_received > 0)
    {
        
    }
    else if(bytes_received == 0)
    {
        std::cout << "\nClient closed the connection";
        close(client_socket);
        return;
    }
    else if(bytes_received < 0)
    {
        if(errno != EWOULDBLOCK && errno != EAGAIN)
        {
            // An error has occurred
            std::cout << "\nAn error has occurred during recv() for socket " << client_socket;
            close(client_socket);
            return;
        }else
        {
            // More data to receive
            return;
        }
    }

}

static std::shared_ptr<LinuxServer> buildTCPServer()
{

}
#ifndef tcp_server_H
#define tcp_server_H

#include <string>
#include <memory>
#include <unordered_map>

struct ClientState
{
    public:
    #ifdef PLATFORM_WINDOWS
        SOCKET m_client_socket;
    #elif defined(PLATFORM_LINUX)
        int m_client_socket;
    #elif defined(PLATFORM_MAC)
        SOCKET m_client_socket;
    #endif

    std::string m_recv_buffer;
    std::string m_send_buffer;
    ssize_t m_bytes_sent;
    bool m_waiting_for_send;
    bool m_waiting_for_recv;
    bool m_is_recv_complete;
    bool m_is_header_complete;
    std::unordered_map<std::string, std::string> m_header;
    std::string m_body;
    size_t m_content_length;
    std::string m_http_method;
    std::string m_res_path;
    std::string m_http_version;

#ifdef PLATFORM_WINDOWS
    ClientState() : m_client_socket{nullptr}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}
    
    ClientState(int client_socket) : m_client_socket{client_socket}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}
#elif defined(PLATFORM_LINUX)
    ClientState() : m_client_socket{-1}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}

    ClientState(int client_socket) : m_client_socket{client_socket}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}
#elif defined(PLATFORM_MAC)
    ClientState() : m_client_socket{-1}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}

    ClientState(int client_socket) : m_client_socket{client_socket}, m_bytes_sent(0), m_waiting_for_send(false), m_waiting_for_Recv(false),
    m_is_recv_complete(false), m_is_header_complete(false) {}
#endif

    void clear()
    {
        m_bytes_sent = 0;
        m_recv_buffer.clear();
        m_send_buffer.clear();
        m_waiting_for_send = false;
        m_waiting_for_recv = false;
        m_is_recv_complete = false;
        m_is_header_complete = false;
        m_header.clear();
        m_body.clear();
        m_content_length = 0;
        m_http_method.clear();
        m_res_path.clear();
        m_http_version.clear();
    }
};

class TCPServer{
    public:
        virtual bool initialize(int port, const std::string& ip_address = "127.0.0.1") = 0;
        virtual void start() = 0; // Start the server and listen for connections
        virtual ~TCPServer() = default;
        
        static std::shared_ptr<TCPServer> buildTCPServer();
};


#endif
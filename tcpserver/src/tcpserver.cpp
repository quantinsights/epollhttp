#include "tcpserver.h"

#ifdef PLATFORM_WINDOWS
#include "win32/win32_server.hpp"
#elif PLATFORM_LINUX
#include "linux/linux_server.hpp"
#elif PLATFORM_MAC
#include "mac/mac_server.hpp"
#endif

std::shared_ptr<TCPServer> TCPServer::buildTCPServer(){
    #ifdef PLATFORM_WINDOWS
        return std::make_shared<Win32Server>();
    #elif PLATFORM_LINUX
        return std::make_shared<LinuxServer>();
    #elif PLATFORM_MAC
        return std::make_shared<MacServer>();
    #endif
}
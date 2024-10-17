#Include OS-specific implementation
project(tcpserver)

if(WIN32)
    set(OS_SPECIFIC_SRC win32/win32_server.cpp)
elseif(UNIX AND NOT APPLE)
    set(OS_SPECIFIC_SRC linux/linux_server.cpp)
elseif(APPLE)
    set(OS_SPECIFIC_SRC mac/mac_server.cpp)
endif()

set(SOURCES
    src/tcpserver.cpp
)

include_directories(include)

# Add tcpserver.cpp
add_library(tcpserver STATIC src/tcpserver.cpp ${OS_SPECIFIC_SRC})

target_include_directories(tcpserver PUBLIC ${MAKE_CURRENT_SOURCE_DIR}/include) 
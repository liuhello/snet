
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
#include <cstring>
#include <sys/unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace snet
{
    class Socket
    {
    public:
        Socket();
        ~Socket();
        bool setAddress(const char* host,int port);
        bool connect();
        std::string getAddress();
        bool setUp(int fd, struct sockaddr *addr);
        bool close();
        bool isClosed(){return m_fd <= 0;}
        int read(void* buf,int size);
        int write(const void* buf,int size);
        int getFd(){return m_fd;}
        bool setTcpNoDelay(bool noDelay);
    protected:
        bool check_socket_handle();
    protected:
        int m_fd;
        struct sockaddr_in m_addr;
    };
    
    class ServerSocket : public Socket
    {
    public:
        ServerSocket(int backlog);
        bool listen();
        Socket* accept();
    protected:
        int m_backlog;
    };
}
#endif

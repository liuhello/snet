
#include "socket.h"

#include <cstdio>
#include <errno.h>
#include <netinet/tcp.h>

namespace snet
{
    ///////////////////////////////////
    //Socket
    //////////////////////////////////
    Socket::Socket()
    {
        m_fd = -1;
        memset(&m_addr,0,sizeof(struct sockaddr_in));
    }
    Socket::~Socket()
    {
        close();
    }
    bool Socket::setAddress(const char* ip,int port)
    {
        memset(&m_addr,0,sizeof(struct sockaddr_in));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        bool rc = true;
        if (ip == NULL || ip[0] == '\0') {
            m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            char c;
            const char *p = ip;
            bool isIPAddr = true;
            // 是ip地址格式吗?
            while ((c = (*p++)) != '\0') {
                if ((c != '.') && (!((c >= '0') && (c <= '9')))) {
                    isIPAddr = false;
                    break;
                }
            }

            if (isIPAddr) {
                m_addr.sin_addr.s_addr = inet_addr(ip);
            } else {
                // 是域名，解析一下
                struct hostent *myHostEnt = gethostbyname(ip);

                if (myHostEnt != NULL) {
                    memcpy(&(m_addr.sin_addr), *(myHostEnt->h_addr_list),
                           sizeof(struct in_addr));
                } else {
                    rc = false;
                }
            }
        }
        return rc;
    }
    bool Socket::connect()
    {
        if(!check_socket_handle())return false;
        int ret = ::connect(m_fd,(struct sockaddr*)&m_addr,sizeof(m_addr));
        return ret == 0;
    }
    bool Socket::check_socket_handle()
    {
        if(m_fd < 0 )
        {
            m_fd = ::socket(AF_INET,SOCK_STREAM,0);
            return m_fd > 0;
        }
        return true;
    }
    std::string Socket::getAddress()
    {
        char dest[32];
        unsigned long ad = ntohl(m_addr.sin_addr.s_addr);
        sprintf(dest, "%d.%d.%d.%d:%d",
                static_cast<int>((ad >> 24) & 255),
                static_cast<int>((ad >> 16) & 255),
                static_cast<int>((ad >> 8) & 255),
                static_cast<int>(ad & 255),
                ntohs(m_addr.sin_port));
        return dest;
    }
    bool Socket::setUp(int fd, struct sockaddr *addr)
    {
        close();
        m_fd = fd;
        memcpy(&m_addr,addr,sizeof(m_addr));
        return true;
    }
    bool Socket::close()
    {
        if(m_fd > 0)
        {
            int ret = ::close(m_fd);
            m_fd = -1;
            return ret == 0;
        }
        return true;
    }
    int Socket::read(void* buf,int len)
    {
        if(m_fd < 0)return -1;
        int res = 0;
        do
        {
            res = ::read(m_fd,buf,len);
        }while(res < 0&&errno == EINTR);
        return res;
    }
    int Socket::write(const void* buf,int len)
    {
        if(m_fd < 0) return -1;
        int res = 0;
        do
        {
            res = ::write(m_fd,buf,len);
        }while(res < 0&&errno == EINTR);
        return res;
    }
    
    bool Socket::setTcpNoDelay(bool noDelay)
    {
        bool rc = false;
        int noDelayInt = noDelay ? 1 : 0;
        if (check_socket_handle()) {
            rc = (setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY,
                             (const void *)(&noDelayInt), sizeof(noDelayInt)) == 0);
        }
        return rc;
    }
    bool Socket::setIntOption(int op,int value)
    {
        int ret = -1;
        if(check_socket_handle()) {
            ret = setsockopt(m_fd,SOL_SOCKET,op,(const void*)(&value),sizeof(value));
        }
        return ret == 0; 
    }
    /////////////////////////////////////////
    //ServerSocket
    /////////////////////////////////////////
    ServerSocket::ServerSocket(int backlog):Socket(),m_backlog(backlog){}
    bool ServerSocket::listen()
    {
        if(!check_socket_handle()) return false;
        int ret = ::bind(m_fd,(struct sockaddr*)&m_addr,sizeof(m_addr));
        if(ret != 0)
        {
            return false;
        }
        ret = ::listen(m_fd,m_backlog);
        if(ret != 0)
        {
            return false;
        }
        return true;
    }
    Socket* ServerSocket::accept()
    {
        Socket* socket = NULL;
        struct sockaddr_in addr;
        int len = sizeof(addr);
        int fd = ::accept(m_fd,(struct sockaddr*)&addr, (socklen_t*)&len);
        if(fd < 0)
        {
            return socket;
        }
        socket = new Socket;
        socket->setUp(fd,(struct sockaddr *)&addr);
        return socket;
    }
}

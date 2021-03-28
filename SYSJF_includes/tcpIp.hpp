#ifndef TCPIP_HPP_INCLUDED
#define TCPIP_HPP_INCLUDED


/*    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket(s) close(s)*/

#include <winsock2.h>
#include <Ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "tram.hpp"

namespace SYSJF
{
    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
    {
    struct sockaddr_storage ss;
    unsigned long s = size;

    ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;

    switch(af) {
        case AF_INET:
        ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
        break;
        case AF_INET6:
        ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
        break;
        default:
        return NULL;
    }
    /* cannot direclty use &size because of strict aliasing rules */
    return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0)?
            dst : NULL;
    }


    class ClientConnection
    {
        public:
        ///heritage de la class d'erreur

        private:
            SOCKADDR_IN m_clientStruct;
            std::shared_ptr<SOCKET> m_clientSocket;

            bool m_debug;
            bool m_debug2;

        public:

        ClientConnection(void)
        {
            this->m_debug = false;
            this->m_debug2 = false;
        }

        ~ClientConnection(){};

        public:

        SOCKADDR_IN & getSockAddrIn(void)
        {
            return this->m_clientStruct;
        }

        std::shared_ptr<SOCKET> & getSocket(void)
        {
            return this->m_clientSocket;
        }

        std::string getIpAddr(void)
        {
            char buff[46] = { 0 };
            return inet_ntop(this->m_clientStruct.sin_family, (void*)&(this->m_clientStruct.sin_addr), buff, 46);
        }

        int getPort(void) const
        {
            return this->m_clientStruct.sin_port;
        }

        void Write(VCHAR const &buffer)
        {
            send(*this->m_clientSocket,(char*)buffer.data(),buffer.size(),0);
        }

        template<unsigned int octets>int Read(VCHAR &buffer)
        {
            char buf[octets];
            int lenght= recv(*this->m_clientSocket ,buf, octets,0);
            buffer.clear();

            for(int i=0;i<lenght;i++)
                buffer.push_back(buf[i]);

            if(lenght == 0)
                Error::add_to_log("connection to " + this->getIpAddr() + ":" + SYSJF::ss_cast<int , std::string>( this->getPort()) +" closed");

            return lenght;
        }

        void forcedAllDebug(bool stat)
        {
            this->m_debug = stat;
            this->m_debug2 = stat;
        }

        inline int getDebugLevel(void)
        {
            return this->m_debug2 ? 2 : this->m_debug ? 1 : 0;
        }
    };

    class CSocketTCPServeur
    {
        public:

        ///heritage de la class d'erreur
        class CSocketTCPServeurError : public Error
        {
            public:
                CSocketTCPServeurError(int numero, std::string const& _str,level _level, int _debugLevel = 0)noexcept:Error(numero,_str,_level,_debugLevel){this->m_class="CSocketTCPServeur";};
                virtual ~CSocketTCPServeurError(){};
        };

        CSocketTCPServeur(void)
        {
            WSAStartup(MAKEWORD(2,0), &this->WSAData);
            this->m_debug = false;
            this->m_debug2 = false;
        }

        ~CSocketTCPServeur(void)
        {
            WSACleanup();
        }

        void NewSocket(unsigned int const &idx)
        {
            Sk_Channel[idx]=std::shared_ptr<SOCKET>(new SOCKET(socket(AF_INET,SOCK_STREAM,0)));

            if(*Sk_Channel[idx]==INVALID_SOCKET)
                throw CSocketTCPServeur::CSocketTCPServeurError(1,"Socket Serveur erreur",Error::level::ERR );
        }

        void CloseSocket(unsigned int const & idx)
        {
            std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
            it=Sk_Channel.find(idx);

            if(it!=Sk_Channel.end())
            {
                closesocket(*Sk_Channel[idx]);
                Sk_Channel.erase(it);
            }
        }

        void BindServeur(unsigned int const  idx,uint32_t const addr,uint32_t const port)
        {
            std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
            it=Sk_Channel.find(idx);

            if(it==Sk_Channel.end())
                throw CSocketTCPServeur::CSocketTCPServeurError(2,"Socket ("+ ss_cast<unsigned int,std::string>(idx)+") inutilisable",Error::level::ERR);

            this->ServerAdress.sin_addr.s_addr=addr;
            this->ServerAdress.sin_family=AF_INET;
            this->ServerAdress.sin_port=htons(port);

            if(bind(*Sk_Channel[idx],(struct sockaddr *)&ServerAdress,sizeof(ServerAdress))==SOCKET_ERROR)
                throw CSocketTCPServeur::CSocketTCPServeurError(3,"le server n'a pas reussi a bind sur le port : "+ss_cast<uint32_t,std::string>(port),Error::level::ERR);
        }
        void Listen(unsigned int const idx,unsigned int const nb)
        {
            std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
            it=Sk_Channel.find(idx);

            if(it==Sk_Channel.end())
                throw CSocketTCPServeur::CSocketTCPServeurError(4,"Socket ("+ ss_cast<unsigned int,std::string>(idx)+") inutilisable",Error::level::ERR);

            listen(*Sk_Channel[idx],nb);
        }

        ClientConnection AcceptClient(unsigned int const idx)
        {
            std::map<unsigned int,std::shared_ptr<SOCKET>>::iterator it;
            it=Sk_Channel.find(idx);

            if(it==Sk_Channel.end())
                throw CSocketTCPServeur::CSocketTCPServeurError(5,"Socket ("+ ss_cast<unsigned int,std::string>(idx)+") inutilisable",Error::level::ERR);

            ClientConnection client;

            int cu=sizeof(struct sockaddr_in);
            client.getSocket()=std::shared_ptr<SOCKET>(new SOCKET(accept(*Sk_Channel[idx],(struct sockaddr *)&client.getSockAddrIn(),&cu)));

            if(*client.getSocket()<0)
                throw CSocketTCPServeur::CSocketTCPServeurError(6,"la connection n'a pas pue etre établie",Error::level::ERR);

            Error::add_to_log("connecting to " + client.getIpAddr() + ":" + ss_cast<int , std::string>( client.getPort()),this->getDebugLevel());

            return client;
        }

        void forcedAllDebug(bool stat)
        {
            this->m_debug = stat;
            this->m_debug2 = stat;
        }

        inline int getDebugLevel(void)
        {
            return this->m_debug2 ? 2 : this->m_debug ? 1 : 0;
        }

    private:

        std::map<unsigned int,std::shared_ptr<SOCKET>> Sk_Channel;
        SOCKADDR_IN ServerAdress;

        WSADATA WSAData;

        bool m_debug;
        bool m_debug2;
    };
};
#endif

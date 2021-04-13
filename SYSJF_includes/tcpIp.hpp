#ifndef TCPIP_HPP_INCLUDED
#define TCPIP_HPP_INCLUDED


#ifdef __linux__
    #include <sys/types.h>
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
    #define closesocket(s) close(s)
#endif

#ifdef WIN32
    #include <winsock2.h>
    #include <Ws2tcpip.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include "tram.hpp"



namespace SYSJF
{
    #ifdef WIN32
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
    #endif




    class CSocketTCPType
    {
        public:
        ///heritage de la class d'erreur
        class CSocketTCPTypeError : public Error
        {
            public:
                CSocketTCPTypeError(int numero, std::string const& _str,level _level, int _debugLevel = 0)noexcept:Error(numero,_str,_level,_debugLevel){this->m_class="CSocketTCPType";};
                virtual ~CSocketTCPTypeError(){};
        };

        public:
            struct t_connect
            {
                std::string addr;
                uint32_t port;
            };

            enum typeip{HOSTNAME,IP};

        protected:
            SOCKADDR_IN m_Struct;
            std::shared_ptr<SOCKET> m_Socket;

            bool m_debug;
            bool m_debug2;

            #ifdef WIN32
                //WSADATA WSAData;
            #endif

        public:

            CSocketTCPType(void);

            virtual ~CSocketTCPType(void);

            SOCKADDR_IN & getSockAddrIn(void);

            void NewSocket(void);

            void CloseSocket(void);

            std::shared_ptr<SOCKET> & getSocket(void);

            std::string getIpAddr(void);

            int getPort(void) const;

            void forcedAllDebug(bool stat);

            int getDebugLevel(void);
    };




    class CSocketTCPConnection : public CSocketTCPType
    {
        public:
        ///heritage de la class d'erreur
        class CSocketTCPConnectionError : public CSocketTCPTypeError
        {
            public:
                CSocketTCPConnectionError(int numero, std::string const& _str,level _level, int _debugLevel = 0)noexcept:CSocketTCPTypeError(numero,_str,_level,_debugLevel){this->m_class="CSocketTCPConnection";};
                virtual ~CSocketTCPConnectionError(){};
        };

        public:

        struct t_connect
        {
            std::string addr;
            uint32_t port;
        };

        enum typeip{HOSTNAME,IP};

        public:
        
        CSocketTCPConnection(void);

        void Connect(struct t_connect const & tc,typeip const & tip);

        void Write(VCHAR const &buffer);

        template<unsigned int octets>int Read(VCHAR &buffer);

    };




    class CSocketTCPServeur : public CSocketTCPType
    {
        public:

        ///heritage de la class d'erreur
        class CSocketTCPServeurError : public CSocketTCPTypeError
        {
            public:
                CSocketTCPServeurError(int numero, std::string const& _str,level _level, int _debugLevel = 0)noexcept:CSocketTCPTypeError(numero,_str,_level,_debugLevel){this->m_class="CSocketTCPServeur";};
                virtual ~CSocketTCPServeurError(){};
        };

        CSocketTCPServeur(void);

        void BindServeur(uint32_t const addr,uint32_t const port);

        void Listen(unsigned int const nb);

        CSocketTCPConnection AcceptClient(void);

    };


/*************************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
***************************************************************************************************************************/


    CSocketTCPType::CSocketTCPType(void):m_Socket(nullptr)
    {
        #ifdef WIN32
            //WSA 2.2
            //WSAStartup(MAKEWORD(2,2), &this->WSAData);
            
        #endif
        
        this->m_debug = false;
        this->m_debug2 = false;
    }

    CSocketTCPType::~CSocketTCPType(void)
    {
        this->CloseSocket();   
        #ifdef WIN32
            //WSACleanup();
        #endif
    }

    SOCKADDR_IN & CSocketTCPType::getSockAddrIn(void)
    {
        return this->m_Struct;
    }

    void CSocketTCPType::NewSocket(void)
    {
        this->m_Socket.reset(new SOCKET(socket(AF_INET,SOCK_STREAM,0)));

        if(*this->m_Socket==INVALID_SOCKET)
            throw CSocketTCPTypeError(1,"Socket Serveur error",Error::level::ERR );
    }

    void CSocketTCPType::CloseSocket(void)
    {
        if(this->m_Socket != nullptr)
            closesocket(*this->m_Socket);

        this->m_Socket = nullptr;
    }

    std::shared_ptr<SOCKET> & CSocketTCPType::getSocket(void)
    {
        return this->m_Socket;
    }

    std::string CSocketTCPType::getIpAddr(void)
    {
        char buff[46] = { 0 };
        return inet_ntop(this->m_Struct.sin_family, (void*)&(this->m_Struct.sin_addr), buff, 46);
    }

    int CSocketTCPType::getPort(void) const
    {
        return this->m_Struct.sin_port;
    }

    void CSocketTCPType::forcedAllDebug(bool stat)
    {
        this->m_debug = stat;
        this->m_debug2 = stat;
    }

    int CSocketTCPType::getDebugLevel(void)
    {
        return this->m_debug2 ? 2 : this->m_debug ? 1 : 0;
    }

/*************************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
***************************************************************************************************************************/


    CSocketTCPConnection::CSocketTCPConnection(void) : CSocketTCPType()
    {
    }

    void CSocketTCPConnection::Connect(struct t_connect const & tc,typeip const & tip)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPConnection::CSocketTCPConnectionError(3,"Connect impossible car pas de socket",Error::level::ERR);

        if(tip==this->IP)
            this->m_Struct.sin_addr.s_addr=inet_addr(tc.addr.c_str());
        else if(tip==this->HOSTNAME)
        {
            struct hostent * hostinfo = nullptr;
            hostinfo = gethostbyname((const char*)tc.addr.c_str());

            if (hostinfo == nullptr)
                throw CSocketTCPConnectionError(1,"server name not found : " + tc.addr,Error::level::ERR);

            this->m_Struct.sin_addr=*(IN_ADDR *) hostinfo->h_addr;
        }

        this->m_Struct.sin_family=AF_INET;
        this->m_Struct.sin_port=htons(tc.port);

        if (connect(*this->m_Socket , (struct sockaddr *)&this->m_Struct , sizeof(this->m_Struct)) == SOCKET_ERROR)
            throw CSocketTCPConnectionError(2,"echec of connection to server",Error::level::ERR);
    }

    void CSocketTCPConnection::Write(VCHAR const &buffer)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPConnection::CSocketTCPConnectionError(3,"Write impossible car pas de socket",Error::level::ERR);

        send(*this->m_Socket,(char*)buffer.data(),buffer.size(),0);
    }

    template<unsigned int octets>int CSocketTCPConnection::Read(VCHAR &buffer)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPConnection::CSocketTCPConnectionError(3,"Read impossible car pas de socket",Error::level::ERR);

        char buf[octets];
        int lenght= recv(*this->m_Socket ,buf, octets,0);
        buffer.clear();

        for(int i=0;i<lenght;i++)
            buffer.push_back(buf[i]);

        if(lenght == 0)
            CSocketTCPConnectionError::add_to_log("connection to " + this->getIpAddr() + ":" + SYSJF::ss_cast<int , std::string>( this->getPort()) +" closed");

        return lenght;
    }

/*************************************************************************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
***************************************************************************************************************************/

    CSocketTCPServeur::CSocketTCPServeur(void):CSocketTCPType()
    {
    }

    void CSocketTCPServeur::BindServeur(uint32_t const addr,uint32_t const port)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPServeur::CSocketTCPServeurError(3,"le server n'a pas reussi a bind sur le port : "+ss_cast<uint32_t,std::string>(port) + " car pas de socket",Error::level::ERR);

            
        this->m_Struct.sin_addr.s_addr=addr;
        this->m_Struct.sin_family=AF_INET;
        this->m_Struct.sin_port=htons(port);

        if(bind(*this->m_Socket,(struct sockaddr *)&this->m_Struct,sizeof(this->m_Struct))==SOCKET_ERROR)
            throw CSocketTCPServeur::CSocketTCPServeurError(3,"le server n'a pas reussi a bind sur le port : "+ss_cast<uint32_t,std::string>(port),Error::level::ERR);
    }

    void CSocketTCPServeur::Listen(unsigned int const nb)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPServeur::CSocketTCPServeurError(3,"listen impossible car pas de socket",Error::level::ERR);

        listen(*this->m_Socket,nb);
    }

    CSocketTCPConnection CSocketTCPServeur::AcceptClient(void)
    {
        if(this->m_Socket == nullptr)
            throw CSocketTCPServeur::CSocketTCPServeurError(3,"aceptClient impossible car pas de socket",Error::level::ERR);

        CSocketTCPConnection client;

        int cu=sizeof(struct sockaddr_in);

        #ifdef WIN32
            client.getSocket()=std::shared_ptr<SOCKET>(new SOCKET(accept(*this->m_Socket,(struct sockaddr *)&client.getSockAddrIn(),&cu)));
        #endif

        #ifdef __linux__
            client.getSocket()=std::shared_ptr<SOCKET>(new SOCKET(accept(*this->m_Socket,(struct sockaddr *)&client.getSockAddrIn(),(socklen_t *)&cu)));
        #endif

        if(*client.getSocket()<0)
            throw CSocketTCPServeur::CSocketTCPServeurError(6,"la connection n'a pas pue etre établie",Error::level::ERR);

        Error::add_to_log("connecting to " + client.getIpAddr() + ":" + ss_cast<int , std::string>( client.getPort()),this->getDebugLevel());

        return client;
    }
};

#endif

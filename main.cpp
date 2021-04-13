#include "SYSJF_includes/main.hpp"
#include "SYSJF_includes/tcpIp.hpp"
#include <thread>
#include <vector>

void RuntimeClient(SYSJF::CSocketTCPConnection  client )
{
    while(true)
    {
        SYSJF::VCHAR resp;

        try
        {
            if(client.Read<4096>(resp) == 0 )
                break;

            std::cout <<"\treceived: "<<resp.size() << "bytes by " << client.getIpAddr() << ":" << client.getPort()<<std::endl;
            std::cout << "\tdata: " << std::string(resp.begin(),resp.end()) << std::endl ;
        }
        catch(SYSJF::Error & e)
		{
			std::cerr << e.what() << '\n';
            break;

		}
		catch(const std::exception & e )
		{
		    std::cerr << e.what() << '\n';
		    break;
		}
    }
}

int SYSJF::Main::body(void)
{
    SYSJF::CSocketTCPServeur server;
	server.forcedAllDebug(this->getDebugLevel()>1?true:false);

	const int nMaxClient = 100 ;

	try
	{
		server.NewSocket();
    	server.BindServeur( INADDR_ANY , 1100);
		server.Listen(nMaxClient );
	}
	catch(SYSJF::Error & e)
	{
		std::cerr << e.what() << '\n';

		if(e.get_level() == SYSJF::Error::level::ERR)
			return -1;

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

    std::vector<std::thread> clientThread;

    while(clientThread.size() <= nMaxClient)
    {
		try
		{
			SYSJF::CSocketTCPConnection client = server.AcceptClient();

			clientThread.emplace_back(&RuntimeClient , client  );

			if(!clientThread.back().joinable())
            	clientThread.back().detach();

		}
		catch(SYSJF::Error & e)
		{
			std::cerr << e.what() << '\n';

			if(e.get_level() == SYSJF::Error::level::ERR)
				return -1;
		}
		catch(const std::exception & e )
		{
		    std::cerr << e.what() << '\n';
		}
    }
	return 0;
}


int main(int argc , char * argv[])
{
	SYSJF::Main main(argc , argv);

	main.version = "0.01\tno test version";

	main.forcedAllDebug(true);

	//execute template main and "int SYSJF::Main::body(void)"
	return main.run();
}

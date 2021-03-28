#ifndef MAIN
#define MAIN

#include "arguments.hpp"

#include <memory>

namespace SYSJF
{
    class Main : public Nocopyable
    {
        private:

            std::unique_ptr<ARGV> m_arguments;

            bool m_debug;
            bool m_debug2;
            
        public:

            class MainError :public Error
            {
                public:
                MainError(int numero, std::string const& _str,level _level , int _debugLevel)noexcept:Error(numero,_str,_level ,_debugLevel){this->m_class="MainError";}
                virtual ~MainError()noexcept(true){}
            };

            Main(int argc , char *argv[]);
            Main(void);

            int run(void);

            int getDebugLevel(void);

            void forcedAllDebug(bool stat);

            std::string version;

            private :

            int body(void);

    };

    Main::Main(int argc , char *argv[]):m_debug(false),version("")
    {
        this->m_arguments = std::make_unique<ARGV>(ARGV(argc,argv));
    }

    Main::Main(void):m_debug(false),version("")
    {
        this->m_arguments = std::make_unique<ARGV>(ARGV(0,{}));
    }

    inline int Main::getDebugLevel(void)
    {
        return this->m_debug2 ? 2 : this->m_debug ? 1 : 0;
    }

    void Main::forcedAllDebug(bool stat)
    {
        this->m_debug = stat;
        this->m_debug2 = stat;
    }

    int Main::run(void)
    {
        MainError::add_to_log("Runtime start",0);

        if(this->m_arguments->isMainArg("d") || this->m_arguments->isOptArg("debug"))
        {
            this->m_debug = true;

            MainError::add_to_log("thirst level of debuging set as true" , 2);
        }


        if(this->m_arguments->isMainArg("d2") || this->m_arguments->isOptArg("debug2"))
        {
            this->m_debug = true;
            this->m_debug2 = true;

            MainError::add_to_log("second level of debuging set as true" , 2);
        }

        int statu = this->body();

        MainError::add_to_log("Runtime close and return " + ss_cast<int , std::string>(statu ) +" in statu",this->getDebugLevel());

        return statu;
    }
};


#endif

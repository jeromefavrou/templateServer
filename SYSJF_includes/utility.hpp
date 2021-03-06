/*
coding by Jerome Favrou , propriety of SYSJF (france) N° SIRET 89206560800010
last update in 2020

this code can be used , modificated 
this code cannot be sale

develloper : Jerome favrou ( SYSJF )
maintener :  SYSJF 
*/
#ifndef UTILITY_HPP_INCLUDED
#define UTILITY_HPP_INCLUDED

#include <dirent.h>
#include <vector>
#include <string>
#include <exception>
#include <system_error>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace SYSJF
{
    enum unix_color{BLACK=30,RED=31,GREEN=32,YELLOW=33,BLUE=34,PINK=35,CYAN=36,GREY=37};

    template<class T1,class T2> T2 ss_cast(T1 const & d)
    {
        std::stringstream _ss_cast;

        _ss_cast << d;

        T2 tps;

        _ss_cast >> tps;

        return tps;
    }

    template<unix_color uc> std::string _print(std::string const & msg)
    {
        std::string nwmsg("");

        #ifndef WIN32
        nwmsg+="\033[";
        nwmsg+=ss_cast<unsigned int,std::string>(static_cast<unsigned int>(uc));
        nwmsg+="m";
        nwmsg+=msg;
        nwmsg+="\033[0m";
        #else
        nwmsg=msg;
        #endif // WIN32

        return nwmsg;
    }
    

    // POO template 

    class Nocopyable
    {
    public:
        Nocopyable(const Nocopyable&) = delete;
        Nocopyable& operator=(const Nocopyable&) = delete;

    protected:
        constexpr Nocopyable() = default;
        ~Nocopyable() = default;
    };

};

#endif // UTILITY_HPP_INCLUDED

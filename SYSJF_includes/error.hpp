/*
coding by Jerome Favrou , propriety of SYSJF (france) N° SIRET 89206560800010
coding by Jerome Favrou , propriety of SYSJF (france) N° SIRET 89206560800010
last update in 2020

this code can be used , modificated
this code cannot be sale

develloper : Jerome favrou ( SYSJF )
maintener :  SYSJF
*/

#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <exception>
#include <string>
#include "utility.hpp"
#include <chrono>
#include <ctime>
#include <fstream>

#define log_path ".log"

namespace SYSJF
{

    class Error
    {
    public:
        enum class level{WARN=0, ERR=1, FATAL_ERR,LOG};
    protected:
        int m_numero;
        std::string m_str,m_class;
        level m_level;
        bool m_log_stat;
    public:


        Error(int numero, std::string const& phrase,level _level , int _debugLevel)noexcept ;

        std::string what();

        std::string get_str(void) const;

        std::string get_class(void) const;

        level get_level(void)  const;

        void set_log_stat(bool const &);

        int get_num(void)  const;

        static void add_to_log(std::string const& msg , int debugLevel = 0)
        {
            std::fstream Of(log_path,std::ios::out | std::ios::app);

            std::time_t end_time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            if(Of && !Of.fail() && !Of.bad())
            {
                std::string date = std::ctime(&end_time);
                date.pop_back();
                Of << date << ": " << msg << std::endl;

                if(debugLevel >=2)
                    std::cout << date << ": " << msg << std::endl;

            }

        }

        virtual ~Error() noexcept(true);

    protected:

        std::string level_to_str(void);
    };

    Error::Error(int numero, std::string const& _str,level _level , int _debugLevel =0 )noexcept:m_numero(numero),m_str(_str),m_level(_level),m_class("Error"),m_log_stat(true)
    {

    }
    std::string Error::level_to_str(void)
    {
        switch(this->m_level)
        {
            case level::WARN: return "WARNING"; break;
            case level::ERR: return "ERROR"; break;
            case level::FATAL_ERR: return "FATAL ERROR"; break;
            default : return "UNKNOW";
        }

        return "UNKNOW";
    }
    std::string Error::what()
    {
        std::string str_error("");
        std::string str("");

        switch(this->m_level)
        {
            case level::WARN: str= _print<unix_color::GREEN>("WARNING"); break;
            case level::ERR: str= _print<unix_color::BLUE>("ERROR"); break;
            case level::FATAL_ERR: str= _print<unix_color::RED>("FATAL ERROR"); break;
            case level::LOG: str= "LOG"; break;
            default : str="WARNING";
        }

        str_error="("+this->m_class+") ";
        str_error+=str+" [";
        str_error+=ss_cast<int,std::string>(this->m_numero)+"]: "+this->m_str;

        return str_error;
    }
    std::string Error::get_str(void) const
    {
        return this->m_str;
    }

    std::string Error::get_class(void) const
    {
        return this->m_class;
    }

    int Error::get_num(void) const
    {
        return this->m_numero;
    }

    void Error::set_log_stat(bool const & _stat)
    {
        this->m_log_stat=_stat;
    }

    Error::level Error::get_level(void) const
    {
        return this->m_level;
    }

    Error::~Error() noexcept(true)
    {
        if(!this->m_log_stat)
            return ;

        std::fstream Of(log_path,std::ios::out | std::ios::app);

        std::time_t end_time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        if(Of && !Of.fail() && !Of.bad())
        {
            std::string date = std::ctime(&end_time);
            date.pop_back();
            Of << date <<"(" << this->m_class<< ")[" <<this->m_numero << "][" << this->level_to_str()<< "] " << this->m_str << "\n\r";
        }

    }

};

#endif // ERROR_HPP_INCLUDED

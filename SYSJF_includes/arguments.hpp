/*
coding by Jerome Favrou , propriety of SYSJF (france) N° SIRET 89206560800010
last update in 2021

this code can be used , modificated 
this code cannot be sale

develloper : Jerome favrou ( SYSJF )
maintener :  SYSJF 
*/

#ifndef ARGUMENTS
#define ARGUMENTS

#include <vector>
#include <string>
#include <map>

#include "error.hpp"
#include "utility.hpp"

namespace SYSJF
{

    class ARGV 
    {
        public:

        ARGV(int argc , char *argv[]);
        
        typedef std::vector<std::string> values_t;
        typedef std::map<std::string,values_t> argv_t;

        private:

        std::string  m_progFile;
        std::string  m_progName;

        // [main[opt1,opt2,...],main2[opt1,opt3,...],...]
        argv_t m_MainArg;
        argv_t m_OptArg;

        private :

        values_t getValues(int idx , int max ,char *argv[]);

        public:

        std::string getProgName(void) const ;
        std::string getProgFile(void) const ;

        values_t getValueOptArg(std::string const & argKey) const ;
        argv_t getOptArg(void) const ;
        bool isOptArg(std::string const & argKey);

        values_t getValueMainArg(std::string const & argKey)const ;
        argv_t getMainArg(void)const ;
        bool isMainArg(std::string const & argKey);

    };

    ARGV::ARGV(int argc , char *argv[])
    {
        
        if(argc < 1 )
            return ;

        // file directory of routine
        this->m_progFile = argv[0];

        //extracte programe name of file directory

        #ifdef WIN32
            const char seperatorPath = '\\' ;
        #else
            const char seperatorPath = '/' ;
        #endif

        for(auto i = this->m_progFile.size()-1 ; i >= 0 ; i--)
        {
            if(this->m_progFile[i] == seperatorPath)
            {
                this->m_progName = std::string(this->m_progFile.begin() + i +1,this->m_progFile.end());
                break;
            }
        }

        //extracte Main

        for(auto i = 1 ; i < argc ; i++)
        {
            std::string cur(argv[i]);
            
            

            if(cur.size() > 1 )
            {
                if(cur[0] == '-' && cur[1] != '-')
                {
                    cur.erase(cur.begin(),cur.begin()+1);

                    this->m_MainArg[cur] = this->getValues(i+1 , argc, argv );

                    i = i + this->m_MainArg[cur].size();
                }
                else if(cur[0] == '-' && cur[1] == '-')
                {
                    cur.erase(cur.begin(),cur.begin()+2);
                    
                    this->m_OptArg[cur] = this->getValues(i+1 , argc, argv );

                    i = i + this->m_OptArg[cur].size();
                }
            }
        }

    };

    ARGV::values_t ARGV::getValues(int idx ,int max , char *argv[])
    {
        values_t res;

        for(auto i= idx ; i < max ; i++)
        {
            std::string cur(argv[i]);

            if(cur[0] == '-')
                break;

            else
                res.push_back(cur);
        }

        return res;
    }

    std::string ARGV::getProgName(void) const
    {
        return this->m_progName;
    }

    std::string ARGV::getProgFile(void) const
    {
        return this->m_progFile;
    }

    ARGV::values_t ARGV::getValueOptArg(std::string const & argKey) const 
    {
        return this->m_OptArg.find(argKey)->second;
    }

    ARGV::argv_t ARGV::getOptArg(void) const
    {
        return this->m_OptArg;
    }

    bool ARGV::isOptArg(std::string const & argKey)
    {
        return this->m_OptArg.find(argKey) != this->m_OptArg.end() ? true : false ;
    }

    ARGV::values_t ARGV::getValueMainArg(std::string const & argKey) const 
    {
        return this->m_MainArg.find(argKey)->second;
    }

    ARGV::argv_t ARGV::getMainArg(void) const
    {
        return this->m_MainArg;
    }

    bool ARGV::isMainArg(std::string const & argKey)
    {
        return this->m_MainArg.find(argKey) != this->m_MainArg.end() ? true : false ;
    }

};

#endif
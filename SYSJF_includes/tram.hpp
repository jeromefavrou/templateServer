#ifndef TRAM_HPP_INCLUDED
#define TRAM_HPP_INCLUDED

#include <vector>
#include <string>
#include <sstream>
#include "error.hpp"


namespace SYSJF
{
typedef unsigned char byte;

//typedef std::vector<byte> VCHAR;

    class VCHAR : public std::vector<byte>
    {
        public:
        
        VCHAR():std::vector<byte>()
        {}

        template< class InputIt >constexpr VCHAR( InputIt first, InputIt last ) : std::vector<byte>(first , last){}

        explicit VCHAR( size_type count,const byte& value = byte()) : std::vector<byte>(count , value){}

        std::string c_str(void) const
        {
            return std::string(this->begin() , this->end());
        }

    };

    class Tram
    {
        public:
            class TramError : public Error
            {
            public:
                TramError(int numero, std::string const& _str,level _level ,int _debugLevel =0)noexcept:Error(numero,_str,_level ,_debugLevel){this->m_class="Tram";};
                virtual ~TramError()noexcept(true){};
            };
            //ascii correspondance

            class Com_bytes
            {
            public:
                static byte constexpr SOH=byte(0x01); //Start of Heading
                static byte constexpr EOT=byte(0x04); //End of Transmission
                static byte constexpr ENQ=byte(0x05); //Enquiry
                static byte constexpr ACK=byte(0x06); //Acknowledge
                static byte constexpr NAK=byte(0x15); //Negative Acknowledge
                static byte constexpr US=byte(0x1F); //Unit Separator
                static byte constexpr GS=byte(0x1D); //Group Separator
                static byte constexpr CS=byte(0x00); //nullprt
            };

            Tram(void);
            Tram(Tram const &);
            Tram(std::string const &);
            Tram(VCHAR const &);

            Tram operator=(Tram const & );
            Tram operator=(std::string const &);
            Tram operator=(VCHAR const &);

            Tram operator+(Tram const &);
            Tram operator+(std::string const &);
            Tram operator+(VCHAR const &);
            Tram operator+(byte const &);

            Tram operator+=(Tram const &);
            Tram operator+=(std::string const &);
            Tram operator+=(VCHAR const &);
            Tram operator+=(byte const &);

            void clear() noexcept;
            size_t size() const noexcept;

            VCHAR  & get_data(void);

            VCHAR get_c_data(void) const;

            std::stringstream get_ss_data(void);

            template<typename T> static VCHAR cast_to_vchar(T const & _type)
            {

                VCHAR tps(sizeof(T));

                long fl=*((long *) &_type);

                for(auto i=0u;i<sizeof(T);i++)
                    tps[i]= fl >> 8*(sizeof(T)-1-i) & 0xFF ;

                return tps;
            }
            template<typename T> static T cast_to_type(VCHAR const & _data)
            {
                if(sizeof(T)!=_data.size())
                    throw TramError(0,"cast_to_type(VCHAR const & _data) sizeof(T)!=_data.size() :",TramError::level::ERR );

                T tps(0);

                long fl(0);

                for(auto & d:_data)
                    fl = fl << 8 | d;

                tps  = * ( T * ) &fl;

                return tps;
            }

        private:

            VCHAR m_data;

    };

    Tram::Tram(void){}
    Tram::Tram(Tram const & cpy):m_data(cpy.get_c_data()){}
    Tram::Tram(std::string const & init):m_data(VCHAR(init.begin(),init.end())){}
    Tram::Tram(VCHAR const & init):m_data(init){}

    Tram Tram::operator=(Tram const & cpy)
    {
        this->m_data=cpy.get_c_data();
        return *this;
    }
    Tram Tram::operator=(std::string const & init)
    {
        this->m_data=VCHAR(init.begin(),init.end());
        return *this;
    }
    Tram Tram::operator=(VCHAR const & init)
    {
        this->m_data=init;
        return *this;
    }

    Tram Tram::operator+(Tram const & add)
    {
        Tram t(*this);

        for(auto & i : add.get_c_data())
            t.get_data().push_back(i);

        return t;
    }
    Tram Tram::operator+(std::string const & add)
    {
        Tram t(*this);

        for(auto & i : add)
            t.get_data().push_back(i);

        return t;
    }
    Tram Tram::operator+(VCHAR const & add)
    {
        Tram t(*this);

        for(auto & i : add)
            t.get_data().push_back(i);

        return t;
    }
    Tram Tram::operator+(byte const & add)
    {
        Tram t(*this);

        t.get_data().push_back(add);

        return t;
    }

    Tram Tram::operator+=(Tram const & add)
    {
        for(auto & i : add.get_c_data())
            this->m_data.push_back(i);

        return *this;
    }
    Tram Tram::operator+=(std::string const & add)
    {
        for(auto & i : add)
            this->m_data.push_back(i);

        return *this;
    }
    Tram Tram::operator+=(VCHAR const & add)
    {
        this->m_data.insert(this->m_data.end(),add.begin(),add.end());

        return *this;
    }
    Tram Tram::operator+=(byte const & add)
    {
        this->m_data.push_back(add);

        return *this;
    }

    void Tram::clear() noexcept
    {
        m_data.clear();
    }
    size_t Tram::size() const noexcept
    {
        return this->m_data.size();
    }

    VCHAR  & Tram::get_data(void)
    {
        return this->m_data;
    }

    VCHAR Tram::get_c_data(void) const
    {
        return this->m_data;
    }

    std::stringstream Tram::get_ss_data(void)
    {
        std::stringstream tps;
        tps << std::string(this->m_data.begin(),this->m_data.end());
        return tps;
    }
};
#endif // TRAM_HPP_INCLUDED

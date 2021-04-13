#ifndef __CSV__

#define __CSV__

#include <vector>
#include <fstream>
#include "error.hpp"

namespace SYSJF
{
	class Csv
	{
		public:

			enum class Separator : char{Comma = ',' , Tabulation = '\t' , Quote = '\'' ,Space = ' ' , DotComma = ';'};

			Csv(Separator const separator);
			~Csv(void);

			typedef std::vector<std::string> Line;

			static void addLine(std::string const & file,Separator const separator , Line const & line )
			{	
				std::fstream csv(file , std::ios::app | std::ios::out);

				if(!csv.good())
				{
				}

				for(auto & word : line)
					csv << word << static_cast<char>(separator) ;
				

				csv << std::endl;
			}

		private:

			const Separator m_separator;
			std::vector<Line> m_data;

	};

	Csv::Csv(Csv::Separator const separator):m_separator(separator)
	{
	}
	Csv::~Csv(void)
	{}
};

#endif

#include "utils.h"

void skipNotData(std::ifstream infile, char commentChar)
{
	if(infile.good())
	{
		char c;
		std::string thrash;
		c = infile.peek();
		while(infile.good() && c == commentChar)
		{
			getline(infile, thrash);
		}
	}
}



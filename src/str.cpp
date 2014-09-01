//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "str.hpp"
#include <sstream>
#include <iomanip>

////////////////////////////////////////////////
// Str
////////////////////////////////////////////////
using namespace haystack;

const Str& Str::EMPTY = *(new Str(""));

Str::Str(const std::string &val) : value(val){}

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Return value string.
const std::string Str::to_string() const
{
	return value;
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Str::to_zinc() const
{
	std::stringstream os;
	os << '"';

    for (std::string::const_iterator it = value.begin(), end = value.end(); it != end; ++it)
	{
        int c = *it & 0xFF;
		if (c < ' ' || c > 127 || c == '"' || c == '\\')
		{
			os << '\\';
			switch (c)
			{
				case '\n':  os << 'n';  break;
				case '\r':  os << 'r';  break;
				case '\t':  os << 't';  break;
				case '"':   os << '"';  break;
				case '\\':  os << '\\'; break;
				default:
					os << 'u' << '0' << '0';
					if (c <= 0xf)
						os << '0';
                    os << std::hex << std::setprecision(4) << std::uppercase << c 
                        << std::nouppercase << std::setprecision(0) << std::dec;
			}
		}
		else
		{
			os << ((char)c);
		}
	}

	os << '"';
	return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Str::operator ==(const Str &other) const
{
	return value == other.value;
}

bool Str::operator !=(const Str &other) const
{
	return !(*this == other);
}

bool Str::operator < (const Str &other) const
{
	return value < other.value;
}

bool Str::operator > (const Str &other) const
{
    return value > other.value;
}


bool Str::operator ==(const std::string &other) const
{
	return value == other;
}

////////////////////////////////////////////////
// implicit conversion
////////////////////////////////////////////////
/*Str::operator const std::string() const
{
	return value;
}*/

/*Str::operator const char*() const
{
	return this->value.c_str();
}
*/

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "num.hpp"
#include <sstream>
//#include <iomanip>
#include <cstdio>
#include <limits>
#include <cmath>

////////////////////////////////////////////////
// Num
////////////////////////////////////////////////
using namespace haystack;

const Num Num::ZERO		= Num();
const Num Num::POS_INF	= Num(std::numeric_limits<double>::infinity(), "");
const Num Num::NEG_INF	= Num(-std::numeric_limits<double>::infinity(), "");
const Num Num::NaN		= Num(NAN, "");

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Num::to_zinc() const
{
	std::stringstream os;
	if (value == std::numeric_limits<double>::infinity()) os << "INF";
	else if (value == -std::numeric_limits<double>::infinity()) os << "-INF";
	else if (isnan(value)) os << "NaN";
	else
	{
		// don't let fractions
		double abs = value; if (abs < 0) abs = -abs;
		if (abs > 1.0)
		{
			char buf[64];
			sprintf(buf, "%.6g", value);
			os << buf;
			//os << std::fixed << std::setprecision(4) << value << std::setprecision(0);
		}
		else
		{
			os << value;
		}

		os << unit;
	}
	return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Num::operator ==(const Num &other) const
{
	return value == other.value && unit == other.unit;
}

bool Num::operator !=(const Num &other) const
{
	return !(*this == other);
}

bool Num::operator ==(double other) const
{
	return value == other && unit == "";
}

bool Num::operator ==(int other) const
{
	return value == other && unit == "";
}

bool Num::operator ==(int64_t other) const
{
	return value == other && unit == "";
}

////////////////////////////////////////////////
// Comparation
////////////////////////////////////////////////
bool Num::operator < (const Num &other) const
{
	return value < other.value;
}

bool Num::operator >(const Num &other) const
{
	return value > other.value;
}

////////////////////////////////////////////////
// Static
////////////////////////////////////////////////

// is valid unit
bool Num::isUnitName(const std::string& unit)
{
    for (std::string::const_iterator it = unit.begin(), end = unit.end(); it != end; ++it)
    {
        int c = *it;
        if (c > 31 && c < 128 
            && !(c >= 'a' && c <= 'z') 
            && !(c >= 'A' && c <= 'Z') 
            && c != '_' && c != '$' && c != '%' && c != '/')
            return false;
    }
    return true;
}

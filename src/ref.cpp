//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "ref.hpp"
#include <sstream>

////////////////////////////////////////////////
// Ref
////////////////////////////////////////////////
using namespace haystack;


////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Ref::to_string() const
{
    return value;
}


////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Ref::to_zinc() const
{
	std::stringstream os;
    os << '@';
    os << value;
    if (!static_cast<std::string>(dis).empty())
    {
        os << (' ');
        os << dis.to_zinc();
    }
    return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Ref::operator ==(const Ref &other) const
{
    return value == other.value;
}

bool Ref::operator !=(const Ref &other) const
{
	return !(*this == other);
}


////////////////////////////////////////////////
// Static
////////////////////////////////////////////////


// is valid id
bool Ref::isId(const std::string& id)
{
    for (std::string::const_iterator it = id.begin(), end = id.end(); it != end; ++it)
    {
        int c = *it;
        if ((c > 31 && c < 128) &&
            (      (c >= '0' && c <= '9')
                || (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')
                || (c == '_' || c == ':' || c == '-' || c == '.' || c == '~')
             ))
            continue;
        else
            return false;
    }
    return true;
}

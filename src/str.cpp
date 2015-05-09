//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "str.hpp"
#include <sstream>
#include <iomanip>
#include <stdint.h>

////////////////////////////////////////////////
// Str
////////////////////////////////////////////////
using namespace haystack;

const Str& Str::EMPTY = Str("");

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
        if (c < ' ' || c == '"' || c == '\\')
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
                os << std::hex << std::uppercase << c
                    << std::nouppercase << std::dec;
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

bool Str::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Str&>(other).operator==(*this);
}

bool Str::operator < (const Val &other) const
{
    return type() == other.type() && value < ((Str&)other).value;
}

bool Str::operator >(const Val &other) const
{
    return type() == other.type() && value > ((Str&)other).value;
}


bool Str::operator ==(const std::string &other) const
{
    return value == other;
}

Str::auto_ptr_t Str::clone() const
{
    return auto_ptr_t(new Str(*this));
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

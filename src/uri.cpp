//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "uri.hpp"
#include <sstream>
#include <stdexcept>

////////////////////////////////////////////////
// Uri
////////////////////////////////////////////////
using namespace haystack;

const Uri Uri::EMPTY = Uri("");

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Return value string.
const std::string Uri::to_string() const
{
    return value;
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Uri::to_zinc() const
{
    std::stringstream os;
    os << '`';

    for (std::string::const_iterator it = value.begin(), end = value.end(); it != end; ++it)
    {
        int c = *it;
        if ((c & 0xFF) < ' ') throw std::runtime_error("Invalid URI char.");
        if (c == '`') os << '\\';
        os << (char)c;
    }
    os << '`';

    return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Uri::operator ==(const Uri &other) const
{
    return value == other.value;
}

bool Uri::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Uri&>(other).operator==(*this);
}

bool Uri::operator !=(const Uri &other) const
{
    return !(*this == other);
}

bool Uri::operator < (const Val &other) const
{
    return type() == other.type() && value < ((Uri&)other).value;
}

bool Uri::operator >(const Val &other) const
{
    return type() == other.type() && value > ((Uri&)other).value;
}


bool Uri::operator ==(const std::string &other) const
{
    return value == other;
}

Uri::auto_ptr_t Uri::clone() const
{
    return auto_ptr_t(new Uri(*this));
}
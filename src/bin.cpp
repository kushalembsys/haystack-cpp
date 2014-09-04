//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "bin.hpp"
#include <sstream>
#include <stdexcept>

////////////////////////////////////////////////
// Bin
////////////////////////////////////////////////
using namespace haystack;

Bin::Bin(const std::string &val) : value(val)
{
    if (val.size() == 0 || val.find('/') == val.npos)
        throw std::runtime_error("Invalid mime val: \"" + val + "\"");
}

// MIME type for binary file
const std::string Bin::to_string() const
{
    return value;
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "Bin(<mime>)"
const std::string Bin::to_zinc() const
{
    std::stringstream os;
    os << "Bin(";

    for (std::string::const_iterator it = value.begin(), end = value.end(); it != end; ++it)
    {
        int c = *it;
        if (c > 127 || c == ')')
        {
            std::stringstream ss;
            ss << "Invalid mime, char='" << (char)c << "'";
            throw std::runtime_error(ss.str().c_str());
        }
        os << (char)c;
    }
    os << ')';
    return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Bin::operator ==(const Bin &other) const
{
    return value == other.value;
}

bool Bin::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Bin&>(other).operator==(*this);
}

bool Bin::operator !=(const Bin &other) const
{
    return value != other.value;
}
bool Bin::operator ==(const std::string &other) const
{
    return value == other;
}

bool Bin::operator < (const Val &other) const
{
    return type() == other.type() && value < ((Bin&)other).value;
}

bool Bin::operator >(const Val &other) const
{
    return type() == other.type() && value > ((Bin&)other).value;
}

Bin::auto_ptr_t Bin::clone() const
{
    return auto_ptr_t(new Bin(*this));
}
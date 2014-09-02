//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "bool.hpp"

////////////////////////////////////////////////
// Bool
////////////////////////////////////////////////
using namespace haystack;

Bool::Bool(bool val) : value(val){}

const Bool Bool::TRUE_VAL = Bool(true);
const Bool Bool::FALSE_VAL = Bool(false);

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Encode as "true" or "false"
const std::string Bool::to_string() const
{
	return value ? "true" : "false";
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "T" or "F"
const std::string Bool::to_zinc() const
{
	return value ? "T" : "F";
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////

bool Bool::operator == (const Bool &other) const
{
    return value == other.value;
}

bool Bool::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Bool&>(other).operator==(*this);
}

bool Bool::operator != (const Bool &other) const
{
    return !(*this == other);
}

bool Bool::operator < (const Bool &other) const
{
    return value < other.value;
}

bool Bool::operator > (const Bool &other) const
{
    return value > other.value;
}

Bool::auto_ptr_t Bool::clone() const
{
    return auto_ptr_t(new Bool(*this));
}
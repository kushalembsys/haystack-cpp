//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
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

/*Bool::operator bool() const
{
	return value;
}*/
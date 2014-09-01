//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "val.hpp"

////////////////////////////////////////////////
// EmptyVal
////////////////////////////////////////////////
using namespace haystack;

const EmptyVal& EmptyVal::DEF = *(new EmptyVal());

const char EmptyVal::type() const { return EMPTY_TYPE; }

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Encode as "marker"
const std::string EmptyVal::to_string() const
{
    assert(false); return "";
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

const std::string EmptyVal::to_zinc() const
{
    assert(false);  return "";
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool EmptyVal::operator ==(const Val &other) const
{
    return type() == other.type();
}
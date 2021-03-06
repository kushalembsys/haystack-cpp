//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "val.hpp"

using namespace haystack;

////////////////////////////////////////////////
// EmptyVal
////////////////////////////////////////////////

const EmptyVal& EmptyVal::DEF = EmptyVal();

const Val::Type EmptyVal::type() const { return EMPTY_TYPE; }

////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Encode as "marker"
const std::string EmptyVal::to_string() const
{
   return "";
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

const std::string EmptyVal::to_zinc() const
{
   return "";
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool EmptyVal::operator ==(const Val &other) const
{
    return &other == NULL || type() == other.type();
}

////////////////////////////////////////////////
// Cmp
////////////////////////////////////////////////
bool EmptyVal::operator > (const Val&) const { return false; }
bool EmptyVal::operator < (const Val&) const { return false; }

EmptyVal::auto_ptr_t EmptyVal::clone() const
{
    return auto_ptr_t(new EmptyVal());
}
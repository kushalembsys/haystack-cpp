//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "marker.hpp"

////////////////////////////////////////////////
// Marker
////////////////////////////////////////////////
using namespace haystack;

const Marker& Marker::VAL = Marker();


////////////////////////////////////////////////
// to string
////////////////////////////////////////////////

// Encode as "marker"
const std::string Marker::to_string() const
{
    return "marker";
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "M"
const std::string Marker::to_zinc() const
{
    return "M";
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Marker::operator ==(const Marker&) const
{
    // all markers are the same
    return true;
}
bool Marker::operator==(const Val &other) const
{
    return type() == other.type();
}

////////////////////////////////////////////////
// Cmp
////////////////////////////////////////////////
bool Marker::operator > (const Val& other) const
{
    return  type() == other.type() && to_string() > other.to_string();
}
bool Marker::operator < (const Val& other) const
{
    return  type() == other.type() && to_string() < other.to_string();
}


Marker::auto_ptr_t Marker::clone() const
{
    return auto_ptr_t(new Marker());
}
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "coord.hpp"
#include <cstdio>
#include <sstream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

////////////////////////////////////////////////
// Coord
////////////////////////////////////////////////
using namespace haystack;

// private ctor
Coord::Coord(int32_t lat, int32_t lng) : ulat(lat), ulng(lng) 
{
    if (ulat < -90000000 || ulat > 90000000) throw std::runtime_error("Invalid lat > +/- 90");
    if (ulng < -180000000 || ulng > 180000000) throw std::runtime_error("Invalid lng > +/- 180");
}

Coord::Coord(double lat, double lng) : ulat((int32_t)(lat * 1000000.0)), ulng((int32_t)(lng * 1000000.0)) 
{
    if (ulat < -90000000 || ulat > 90000000) throw std::runtime_error("Invalid lat > +/- 90");
    if (ulng < -180000000 || ulng > 180000000) throw std::runtime_error("Invalid lng > +/- 180");
}

////////////////////////////////////////////////
// statics
////////////////////////////////////////////////

// Parse from string fomat "C(lat,lng)" or raise runtime exception
Coord Coord::make(const std::string &s) 
{
    if (!boost::starts_with(s, "C(")) throw std::runtime_error("Parse error");
    if (!boost::ends_with(s, ")")) throw std::runtime_error("Parse error");
    size_t comma = s.find(',');
    if (comma < 3) throw std::runtime_error("Parse error");

    std::string lat = s.substr(2, comma - 2);
    std::string lng = s.substr(comma + 1, s.size() - comma - 2);

    return Coord(boost::lexical_cast<double>(lat), boost::lexical_cast<double>(lng));
}

// Return if given latitude is legal value between -90.0 and +90.0 */
bool Coord::is_lat(double lat) { return -90.0 <= lat && lat <= 90.0; }

// Return if given is longtitude is legal value between -180.0 and +180.0
bool Coord::is_lng(double lng) { return -180.0 <= lng && lng <= 180.0; }

void u_to_str(std::stringstream &s, int ud)
{
    if (ud < 0) { s << '-'; ud = -ud; }
    if (ud < 1000000.0)
    {
        double d = (ud / 1000000.0);
        char buf[64];
        sprintf(buf, "%g", d);
        s << buf;
        if (d == 0)
            s << ".0";
        return;
    }
    std::string x = boost::lexical_cast<std::string>(ud);
    size_t dot = x.size() - 6;
    size_t end = x.size();

    while (end > dot + 1 && x[end - 1] == '0') --end;

    for (size_t i = 0; i < dot; ++i)
        s << x[i];

    s << '.';
    for (size_t i = dot; i < end; ++i) s << x[i];
}


//////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////

// Latitude in decimal degrees
double Coord::lat() const { return ulat / 1000000.0; }

// Longtitude in decimal degrees
double Coord::lng() const { return ulng / 1000000.0; }

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode using double quotes and back slash escapes
const std::string Coord::to_zinc() const
{
	std::stringstream os;
	
    os << "C(";
    u_to_str(os, ulat);
    os << ',';
    u_to_str(os, ulng);
    os << ")";

	return os.str();
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////
bool Coord::operator ==(const Coord &other) const
{
	return ulat == other.ulat && ulng == other.ulng;
}

bool Coord::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const Coord&>(other).operator==(*this);
}

bool Coord::operator < (const Val &other) const
{
    return type() == other.type() 
        && ulat < ((Coord&)other).ulat && ulng >((Coord&)other).ulng;
}

bool Coord::operator >(const Val &other) const
{
    return type() == other.type() 
        && ulat > ((Coord&)other).ulat && ulng > ((Coord&)other).ulng;
}

Coord::auto_ptr_t Coord::clone() const
{
    return auto_ptr_t(new Coord(*this));
}
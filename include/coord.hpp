#pragma once
#include "val.hpp"
#include <stdint.h>

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
/*
 Coord models a geographic coordinate as latitude and longitude.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
class Coord : public Val
{
    Coord();
    Coord(int32_t, int32_t);
	// disable assigment
    Coord& operator=(const Coord &other);
    Coord(const Coord &other) : ulat(other.ulat), ulng(other.ulng) {};
public:
    const char type() const { return COORD_TYPE; }
	
    // Latitude in micro-degrees
    const int32_t ulat;
    // Longitude in micro-degrees
    const int32_t ulng;

    Coord(double, double);
    
    // Parse from string fomat "C(lat,lng)" or raise runtime exception
    static Coord make(const std::string &val);

    // Return if given latitude is legal value between -90.0 and +90.0 */
    static bool is_lat(double lat);

    // Return if given is longtitude is legal value between -180.0 and +180.0
    static bool is_lng(double lng);

    // Latitude in decimal degrees
    double lat() const;

    // Longtitude in decimal degrees
    double lng() const;


	// Encode using double quotes and back slash escapes
	const std::string to_zinc() const;

	// Equality is value based
    bool operator ==(const Coord &other) const;
    bool operator !=(const Coord &other) const;
    bool operator < (const Val &other) const;
    bool operator > (const Val &other) const;

	bool operator ==(const std::string &other) const;
    bool operator==(const Val &other) const;

    auto_ptr_t clone() const;

};
};
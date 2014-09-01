#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Marker is the singleton value for a marker tag. 

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a> 

*/
namespace haystack {
class Marker : public Val
{
	// disable construction
    friend class Dict;
    friend class ZincReader;
	Marker() {};
	// disable copy ctor
	Marker(const Marker&);
	// disable assignment
	Marker& operator=(const Marker &other);
public:
    const char type() const { return MARKER_TYPE; }

    // default marker value
	static const Marker& VAL;
    // construct a new marker on heap
    static const Marker* const DEF();

	// Encode as "marker"
	const std::string to_string() const;

	// Encode as "M"
	const std::string to_zinc() const;

	// Equality
	bool operator ==(const Marker &b) const;
    bool operator==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Marker&>(other).operator==(*this);
    }
};
};
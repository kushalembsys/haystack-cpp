#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Uri models a URI as a string.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Uri : public Val
{
    Uri();
	// disable assigment
    Uri& operator=(const Uri &other);
protected:
    const char type() const { return 'U'; }
public:
    // This value
	const std::string value;
	
    Uri(const std::string &val) : value(val) {};
    Uri(const Uri &other) : value(other.value) {};

    static const Uri EMPTY;
	
    // Return value string.
	const std::string to_string() const;

	// Encode using double quotes and back slash escapes
	const std::string to_zinc() const;

	// Equality is value based
    bool operator ==(const Uri &other) const;
    bool operator !=(const Uri &other) const;
    bool operator < (const Uri &other) const;
    bool operator > (const Uri &other) const;

	bool operator ==(const std::string &other) const;

    bool operator ==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Uri&>(other).operator==(*this);
    }
	
};
};
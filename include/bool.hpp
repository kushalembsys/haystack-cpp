#pragma once
#include "val.hpp"

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Bool defines singletons for true/false tag values.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Bool : public Val
{
	Bool();
	// disable assignment
	Bool& operator=(const Bool &other);
protected:
    const char type() const { return 'B'; }
public:
	// This bool value
	const bool value;

	Bool(bool val);	
	Bool(const Bool &other) : value(other.value){};
	
	// Singleton value for true
	static const Bool TRUE_VAL;

	// Singleton value for false
	static const Bool FALSE_VAL;

	// Encode as "true" or "false"
	const std::string to_string() const;

	// Encode as "T" or "F"
	const std::string to_zinc() const;

    // Equality
    bool operator ==(const Bool &other) const;
    bool operator !=(const Bool &other) const;
    bool operator ==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Bool&>(other).operator==(*this);
    }

    bool operator > (const Bool &other) const;
    bool operator < (const Bool &other) const;
	
    // converts to bool
	//operator bool() const;
};
};
#pragma once
#include "headers.hpp"
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Val is the base class for representing haystack tag
 scalar values as an immutable class.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Val
{
protected:
    Val() {};
	Val(const Val&);
	Val& operator=(const Val &other);
public:
    virtual ~Val() {}
	// disable copy ctor
	// String format is for human consumption only
	virtual const std::string to_string() const
	{
		return to_zinc();
	}
	// Encode value to zinc format
    virtual const std::string to_zinc() const = 0;

    virtual const char type() const = 0;
    virtual bool operator ==(const Val &other) const = 0;
};
};
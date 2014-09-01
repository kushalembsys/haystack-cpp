#pragma once
#include "headers.hpp"
#include <boost/noncopyable.hpp>
#include<assert.h>

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

enum
{
    BIN_TYPE = 'b',
    BOOL_TYPE = 'B',
    COORD_TYPE = 'C',
    DATE_TYPE = 'D',
    DATE_TIME_TYPE = 'd',
    MARKER_TYPE = 'M',
    NUM_TYPE = 'N',
    REF_TYPE = 'R',
    STR_TYPE = 'S',
    TIME_TYPE = 'T',
    URI_TYPE = 'U',
    EMPTY_TYPE = '|'
};

class Val : boost::noncopyable
{
	        // disable copy ctor
public:
    virtual ~Val() {};
	// String format is for human consumption only
    virtual const std::string to_string() const { return to_zinc(); }

	// Encode value to zinc format
    virtual const std::string to_zinc() const = 0;

    virtual const char type() const = 0;
    virtual bool operator==(const Val &other) const = 0;
};

// This class is the "empty" value for all types of Val
class EmptyVal : public Val
{
public:
    const std::string to_string() const;

    const std::string to_zinc() const;
    
    static const EmptyVal &DEF;
    
    const char type() const;
    
    bool operator==(const Val &other) const;


};
};
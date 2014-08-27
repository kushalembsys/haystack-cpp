#pragma once
#include "val.hpp"
#include <stdint.h>
#include <stdexcept>
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Num wraps a 64-bit floating point number and optional unit name.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Num : public Val
{
	Num() : value(0.0), unit("") {};
	// disable assignment
	
protected:
    const char type() const { return 'N'; }
public:
    // This double value
    const double		value;
    // This unit string
    const std::string	unit;
    
    Num(double val, const std::string &unit) : value(val), unit(unit) { enforceUnit(); };
    Num(double val) : value(val), unit("") { enforceUnit(); };
    Num(int val, const std::string &unit) : value(val), unit(unit) { enforceUnit(); };
    Num(int val) : value(val), unit("") { enforceUnit(); };
    Num(int64_t val, const std::string &unit) : value(static_cast<double>(val)), unit(unit) { enforceUnit(); };
    Num(int64_t val) : value(static_cast<double>(val)), unit("") { enforceUnit(); };
    Num(const Num &other) : value(other.value), unit(other.unit) { enforceUnit(); };
    Num operator=(const Num &other) { return Num(other.value, other.unit); };
    // special values
    static const Num ZERO;
	static const Num POS_INF;
	static const Num NEG_INF;
	static const Num NaN;

	// Encode value to zinc format
	const std::string to_zinc() const;

	// Equality
	bool operator ==(const Num &other) const;
	bool operator !=(const Num &other) const;
	bool operator > (const Num &other) const;
	bool operator < (const Num &other) const;
	bool operator ==(double other) const;
	bool operator ==(int other) const;
	bool operator ==(int64_t other) const;
    bool operator ==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Num&>(other).operator==(*this);
    }
    // check if str is a valid unit name
    static bool isUnitName(const std::string&);

    // utils
private:
    void enforceUnit()
    { 
        if (!Num::isUnitName(unit)) 
            throw std::runtime_error("invalid unit"); 
    }
};
};
//
// Copyright (c) 2016, Siemens Building Technolgies
// History:
//   06 Jan 2016  Steve Fritsche, adapted from Bool.hpp
//

#include "gridval.hpp"

////////////////////////////////////////////////
// GridVal
////////////////////////////////////////////////
using namespace haystack;

GridVal::GridVal(){}


////////////////////////////////////////////////
// to string
////////////////////////////////////////////////


const std::string GridVal::to_string() const
{
	return "";
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////


const std::string GridVal::to_zinc() const
{
	return "";
}

////////////////////////////////////////////////
// Equal
////////////////////////////////////////////////

bool GridVal::operator == (const GridVal &other) const
{
    return (*this == other);
}

bool GridVal::operator==(const Val &other) const
{
    if (type() != other.type())
        return false;
    return static_cast<const GridVal&>(other).operator==(*this);
}

bool GridVal::operator < (const Val &other) const
{
    return false;
}

bool GridVal::operator > (const Val &other) const
{
    return false;
}

GridVal::auto_ptr_t GridVal::clone() const
{
    return auto_ptr_t(new GridVal(*this));
}

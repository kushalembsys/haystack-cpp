//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "dict.hpp"

////////////////////////////////////////////////
// Dict
////////////////////////////////////////////////
using namespace haystack;

const Dict Dict::EMPTY = Dict();

// Return true if size is zero
const bool Dict::is_empty() const { return _map.size() == 0; }

// Return number of tag name / value pairs
const size_t Dict::size() const { return _map.size(); }

// Return if the given tag is present
const bool Dict::has(const std::string& name) const { return _map.count(name) > 0; }

// Return if the given tag is not present
const bool Dict::missing(const std::string& name) const { return !has(name); }

// Convenience for "get(name, true)"
const Val& Dict::get(const std::string& name) const
{
    dict_t::const_iterator it = _map.find(name);
    return it->second;
}

// Iteratator to walk each name / tag pair
Dict::dict_t::const_iterator Dict::begin() const
{
    return _map.begin();
}

// End Iteratator
Dict::dict_t::const_iterator Dict::end() const
{
    return _map.end();
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode as "T" or "F"
const std::string Dict::to_zinc() const
{
	return "";
}

// Equality
bool Dict::operator == (const Dict &other) const
{
    if (size() != other.size()) return false;

    for (dict_t::const_iterator it = begin(), e = end(); it != e; ++it)
    {
        if (!(it->second == other.get(it->first))) return false;
    }
    return true;
}


////////////////////////////////////////////////
// DictBuilder
////////////////////////////////////////////////
DictBuilder::DictBuilder()
{
}

DictBuilder::~DictBuilder()
{
}
// Add new tag
DictBuilder& DictBuilder::add(const std::string name, const Val& val)
{
    _map.insert(value_t(name, val));
    return *this;
}
// Returns a dict with the value added
const Dict DictBuilder::to_dict() const
{
    return Dict(_map);
}
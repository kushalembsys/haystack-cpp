#pragma once
#include "val.hpp"
#include <map>
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   26 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Dict is an immutable map of name/Val pairs

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {

class Dict
{
public:
    // dict internal type
    typedef std::map < const std::string, const Val&> dict_t;
private:
    const dict_t _map;
    // disable copy ctor
    friend class DictBuilder;
    Dict& operator=(const Dict &other);
    Dict(){};
    Dict(const Dict &other);// : _map(other._map) {};
    Dict(const dict_t &m) : _map(m) {};
public:
    
    // Singleton for empty set of tags.
    const static Dict EMPTY;

    // Return true if size is zero
    const bool is_empty() const;
    // Return number of tag name / value pairs
    const size_t size() const;
    // Return if the given tag is present
    const bool has(const std::string& name) const;

    // Return if the given tag is not present
    const bool missing(const std::string& name) const;

    // Get a tag by name
    const Val& get(const std::string& name) const;

    // Iteratator to walk each name / tag pair
    dict_t::const_iterator begin() const;
    dict_t::const_iterator end() const;

	// String format is for human consumption only
	const std::string to_string() const
	{
		return to_zinc();
	}
	
    // Encode value to zinc format
	const std::string to_zinc() const;

    // Equality
    bool operator ==(const Dict &b) const;
};

class DictBuilder
{
public:
    DictBuilder();
    ~DictBuilder();

private:
    Dict::dict_t _map;
    typedef std::pair<const std::string, const Val&> value_t;
public:
    // Add new tag
    DictBuilder& add(const std::string name, const Val& val);

    const Dict to_dict() const;

};

};
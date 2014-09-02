#pragma once
#include "val.hpp"
#include <boost/ptr_container/ptr_map.hpp>

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   26 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
/*
 Dict is a map of name/Val pairs

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
class Dict : boost::noncopyable
{
public:
    // dict internal type
    typedef boost::ptr_map < std::string, haystack::Val > dict_t;
    typedef dict_t::const_iterator const_iterator;
private:
    dict_t m_map;
public:
    
    // Singleton for empty set of tags.
    const static Dict& EMPTY;

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
    const_iterator begin() const;
    const_iterator end() const;

	// String format is for human consumption only
	const std::string to_string() const
	{
		return to_zinc();
	}
	
    // Encode value to zinc format
	const std::string to_zinc() const;

    // Get display string for this entity:
    // - dis tag
    // - id tag
    const std::string dis() const;

    // Returns a dict with the value added
    Dict& add(std::string name, const Val* val);

    // Returns a dict with the Marker added
    Dict& add(std::string name);

    // Returns a dict with the Str added
    Dict& add(std::string name, const std::string& val);

    // Returns a dict with the Num added
    Dict& add(std::string name, double val, const std::string &unit = "" );

    // Return if the given string is a legal tag name.  The
    // first char must be ASCII lower case letter.  Rest of
    // chars must be ASCII letter, digit, or underbar.
    static const bool is_tag_name(const std::string &);

    // Equality
    bool operator ==(const Dict &b) const;
    bool operator !=(const Dict &b) const;
};
};
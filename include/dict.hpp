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
        typedef std::auto_ptr<Dict> auto_ptr_t;
        // dict internal type
        typedef boost::ptr_map < std::string, haystack::Val > dict_t;
        typedef dict_t::const_iterator const_iterator;
    public:

        virtual ~Dict(){}

        // Singleton for empty set of tags.
        const static Dict& EMPTY;

        // Return true if size is zero
        virtual const bool is_empty() const;
        // Return number of tag name / value pairs
        virtual const size_t size() const;
        // Return if the given tag is present
        virtual const bool has(const std::string& name) const;

        // Return if the given tag is not present
        virtual const bool missing(const std::string& name) const;

        // Get a tag by name
        virtual const Val& get(const std::string& name, bool checked = true) const;

        // Iteratator to walk each name / tag pair
        const_iterator begin() const;
        const_iterator end() const;

        // String format is for human consumption only
        const std::string to_string() const
        {
            return to_zinc();
        }

        // Encode values to zinc format
        const std::string to_zinc() const;

        // Get display string for this entity:
        // - dis tag
        // - id tag
        const std::string dis() const;

        // Returns a dict with the value added, Val* is owned by this dict
        Dict& add(std::string name, const Val* val);

        // Returns a dict with the value added, Val& is cloned
        Dict& add(std::string name, const Val& val);

        // Returns a dict with the Marker added
        Dict& add(std::string name);

        // Returns a dict with the Str added
        Dict& add(std::string name, const std::string& val);

        // Returns a dict with the Num added
        Dict& add(std::string name, double val, const std::string &unit = "");

        // Returns a dict with the Dict added
        Dict& add(const Dict& other);

        // Clones this Dict and its values
        virtual auto_ptr_t clone();

        // Return if the given string is a legal tag name.  The
        // first char must be ASCII lower case letter.  Rest of
        // chars must be ASCII letter, digit, or underbar.
        static const bool is_tag_name(const std::string &);

        // Equality
        bool operator ==(const Dict &b) const;
        bool operator !=(const Dict &b) const;

        // Members
    private:
        dict_t m_map;
    };
};
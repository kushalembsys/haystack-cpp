#pragma once
#include "str.hpp"
#include <stdexcept>
//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   19 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 Ref wraps a string reference identifier and a optional display name.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
*/
namespace haystack {
class Ref : public Val
{
    Ref();
	// disable assignment
    Ref& operator=(const Ref &other);
    Ref(const Ref &other) : value(other.value), m_dis(other.m_dis) { Ref::enforceId(); };
public:
    const char type() const { return REF_TYPE; }

	// This string value
    const std::string	value;

	
    Ref(const std::string& val) : value(val), m_dis("") { Ref::enforceId(); };
    Ref(const std::string& val, const std::string &unit) : value(val), m_dis(unit) { Ref::enforceId(); };
    
    // Return the val string
    const std::string to_string() const;

    // Encode value to zinc format
	const std::string to_zinc() const;

    // Return display string which is dis field if nont empty, val field otherwise
    const std::string dis() const;

	// Equality
    bool operator ==(const Ref &other) const;
    bool operator !=(const Ref &other) const;
    bool operator==(const Val &other) const
    {
        if (type() != other.type())
            return false;
        return static_cast<const Ref&>(other).operator==(*this);
    }
    
    static bool is_id_char(int c);
    // check if str is a valid id
    static bool isId(const std::string&);

    // utils
private:
    // This Str dis
    const Str m_dis;
    void enforceId()
    {
        if (!Ref::isId(value))
            throw std::runtime_error("invalid id");
    }
};
};
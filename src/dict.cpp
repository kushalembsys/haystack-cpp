//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "dict.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "str.hpp"
#include <sstream>

////////////////////////////////////////////////
// Dict
////////////////////////////////////////////////
using namespace haystack;

// Return true if size is zero
const bool Dict::is_empty() const { return m_map.size() == 0; }

// Return number of tag name / value pairs
const size_t Dict::size() const { return m_map.size(); }

// Return if the given tag is present
const bool Dict::has(const std::string& name) const { return m_map.count(name) > 0; }

// Return if the given tag is not present
const bool Dict::missing(const std::string& name) const { return !has(name); }

// Get a tag by name
const Val& Dict::get(const std::string& name) const
{
    dict_t::const_iterator it = m_map.find(name);
    if (it != end())
        return *it->second;
    return EmptyVal::DEF;
}

// Iteratator to walk each name / tag pair
Dict::const_iterator Dict::begin() const
{
    return m_map.begin();
}

// End Iteratator
Dict::const_iterator Dict::end() const
{
    return m_map.end();
}

////////////////////////////////////////////////
// to zinc
////////////////////////////////////////////////

// Encode values to zinc format
const std::string Dict::to_zinc() const
{
    std::stringstream os;
    bool first = true;

    for (dict_t::const_iterator it = begin(), e = end(); it != e; ++it)
    {
        const std::string& name = it->first;
        const Val* val = it->second;
        if (first) first = false; else os << ' ';
        os << name;
        if (!(*val == Marker::VAL)) { os << ':' << val->to_zinc(); }
    }
    return os.str();
}

// Get display string for this entity:
// - dis tag
// - id tag
const std::string Dict::dis() const
{
    const Val& dis = get("dis");
    if (dis.type() == STR_TYPE)
        return ((Str&)dis).value;

    const Val& id = get("id");
    if (!id.is_empty())
        return ((Ref&)id).dis();

    return "????";
}

// Equality
bool Dict::operator == (const Dict &other) const
{
    if (size() != other.size()) return false;

    for (dict_t::const_iterator it = begin(), e = end(); it != e; ++it)
    {
        if (!(*it->second == other.get(it->first))) return false;
    }
    return true;
}

bool Dict::operator != (const Dict &other) const
{
    return !(*this == other);
}

Dict& Dict::add(std::string name, const Val* val)
{
    std::string k = name;
    Val* d = const_cast<Val*>(val);
    m_map.insert(k, d);
    return *this;
}

Dict& Dict::add(std::string name, const Val& val)
{
    std::string k = name;
    Val* v = (Val*) val.clone().release();
    m_map.insert(k, v);
    return *this;
}

Dict& Dict::add(std::string name)
{
    std::string k = name;
    m_map.insert(k, (Marker*)Marker::VAL.clone().release());
    return *this;
}

Dict& Dict::add(std::string name, const std::string& val)
{
    std::string k = name;
    m_map.insert(k, new Str(val));
    return *this;
}

// Returns a dict with the Num added
Dict& Dict::add(std::string name, double val, const std::string &unit)
{
    std::string k = name;
    m_map.insert(k, new Num(val, unit));
    return *this;
}

// Returns a dict with the Dict added
Dict& Dict::add(const Dict& other)
{
    for (const_iterator it = other.begin(), e = other.end(); it != e; ++it)
    {
        add(it->first, *it->second);
    }
    return *this;
}

// Clones this Dict and its values
Dict::auto_ptr_t Dict::clone()
{
    auto_ptr_t c(new Dict());
    for (const_iterator it = begin(), e = end(); it != e; ++it)
    {
        c->add(it->first, it->second->clone().release());
    }

    return c;
}

////////////////////////////////////////////////
// Static
////////////////////////////////////////////////

const Dict& Dict::EMPTY = *(new Dict());

const bool Dict::is_tag_name(const std::string &n)
{
    if (n.size() == 0) return false;

    int first = n[0] & 0xFF;
    if (first < 'a' || first > 'z') return false;

    for (std::string::const_iterator it = n.begin(), end = n.end(); it != end; ++it)
    {
        int c = *it & 0xFF;
        if ((c > 31 && c < 128) &&
            ((c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c == '_')
            ))
            continue;
        else
            return false;
    }
    return true;
}

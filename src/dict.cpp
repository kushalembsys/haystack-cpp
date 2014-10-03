//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   19 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "dict.hpp"
#include "bool.hpp"
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
const bool Dict::is_empty() const { return size() == 0; }

// Return number of tag name / value pairs
const size_t Dict::size() const { return m_map.size(); }

// Return if the given tag is present
const bool Dict::has(const std::string& name) const
{
    return get(name, false) != EmptyVal::DEF;
}

// Return if the given tag is not present
const bool Dict::missing(const std::string& name) const
{
    return get(name, false) == EmptyVal::DEF;
}

// Get the "id" tag as Ref
const Ref& Dict::id() const
{
    return get_ref("id");
}

// Get a tag by name
const Val& Dict::get(const std::string& name, bool checked) const
{
    dict_t::const_iterator it = m_map.find(name);
    if (it != end())
        return *it->second;

    if (checked)
        throw std::runtime_error("Name not found: " + name);
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
        if (first) 
            first = false;
        else os << ' ';
        
        os << name;
        if (*val != Marker::VAL) 
            os << ':' << val->to_zinc();
    }
    return os.str();
}

// Get display string for this entity:
// - dis tag
// - id tag
const std::string Dict::dis() const
{
    const Val& dis = get("dis", false);
    if (dis.type() == Val::STR_TYPE)
        return ((Str&)dis).value;

    const Val& id = get("id", false);
    if (!id.is_empty())
        return ((Ref&)id).dis();

    return "????";
}

//////////////////////////////////////////////////////////////////////////
// Get Conveniences
//////////////////////////////////////////////////////////////////////////

// Get tag as Bool or raise runtime_exception.
bool Dict::get_bool(const std::string& name) const
{
    const Val& v = get(name);
    if (v.type() == Val::BOOL_TYPE) return ((Bool&)v).value;

    throw std::runtime_error("Value type not a BOOL_TYPE");
}

// Get tag as Str or raise runtime_exception.
const std::string& Dict::get_str(const std::string& name) const
{
    const Val& v = get(name);
    if (v.type() == Val::STR_TYPE) return ((Str&)v).value;

    throw std::runtime_error("Value type not a STR_TYPE");
}
// Get tag as Ref or raise runtime_exception.
const Ref& Dict::get_ref(const std::string& name) const
{
    const Val& v = get(name);
    if (v.type() == Val::REF_TYPE) return ((Ref&)v);

    throw std::runtime_error("Value type not a REF_TYPE");
}

// Get tag as Num or raise runtime_exception.
long long Dict::get_int(const std::string& name) const
{
    return static_cast<long long>(get_double(name));
}

// Get tag as Num or raise runtime_exception.
double Dict::get_double(const std::string& name) const
{
    const Val& v = get(name);
    if (v.type() == Val::NUM_TYPE)
        return ((Num&)v).value;

    throw std::runtime_error("Value type not a NUM_TYPE");
}

// Equality
bool Dict::operator == (const Dict &other) const
{
    if (size() != other.size()) return false;

    for (dict_t::const_iterator it = begin(), e = end(); it != e; ++it)
    {
        if (*it->second != other.get(it->first, false))
            return false;
    }
    return true;
}

bool Dict::operator != (const Dict &other) const
{
    return !(*this == other);
}

Dict& Dict::add(std::string name, Val::auto_ptr_t val)
{
    std::string k = name;
    m_map.insert(k, val);
    return *this;
}

Dict& Dict::add(std::string name, const Val* val)
{
    std::string k = name;
    m_map.insert(k, const_cast<Val*>(val));
    return *this;
}

Dict& Dict::add(std::string name, const Val& val)
{
    std::string k = name;
    m_map.insert(k, new_clone(val));
    return *this;
}

Dict& Dict::add(std::string name)
{
    std::string k = name;
    m_map.insert(k, (Marker*)new_clone(Marker::VAL));
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
        c->add(it->first, it->second->clone());
    }

    return c;
}

////////////////////////////////////////////////
// Static
////////////////////////////////////////////////

const Dict& Dict::EMPTY = Dict();

const bool Dict::is_tag_name(const std::string &n)
{
    if (n.empty()) return false;

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

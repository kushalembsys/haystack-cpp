//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   28 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "col.hpp"
#include "str.hpp"
#include "dict.hpp"

////////////////////////////////////////////////
// Col
////////////////////////////////////////////////
using namespace haystack;

// public:
//////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////

// Return programatic name of column
const std::string Col::name() const { return m_name; }

// Return display name of column which is meta.dis or name
const std::string Col::dis() const
{
    const Val& dis = m_meta->get("dis");
    if (dis.type() == Val::STR_TYPE)
        return ((Str&)dis).value;
    return m_name;
}

// Column meta-data tags
const Dict& Col::meta() const { return *m_meta; }

// Equality is name and meta
bool Col::operator== (const Col& that)
{
    return m_name == that.m_name && *m_meta == *that.m_meta;
}

bool Col::operator!= (const Col& that)
{
    return !(*this == that);
}
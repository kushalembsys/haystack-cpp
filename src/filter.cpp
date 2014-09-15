//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   02 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "filter.hpp"
#include "val.hpp"
#include "ref.hpp"
#include "dict.hpp"
#include "zincreader.hpp"

#include <stdexcept>
#include <sstream>

using namespace haystack;
////////////////////////////////////////////////
// Filter
////////////////////////////////////////////////

Filter::shared_ptr_t Filter::make(const std::string& s, bool checked)
{
    try
    {
        std::istringstream iss(s);
        ZincReader r(iss);
        return r.read_filter();
    }
    catch (std::exception& e)
    {
        if (!checked)
            return shared_ptr_t();
        throw e;
    }
}

// Match records which have the specified tag path defined.
Filter::shared_ptr_t Filter::has(const std::string& path)
{
    return shared_ptr_t(new Has(Path::make(path)));
}

// Match records which do not define the specified tag path.
Filter::shared_ptr_t Filter::missing(const std::string&  path)
{
    return shared_ptr_t(new Missing(Path::make(path)));
}

// Match records which have a tag are equal to the specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::eq(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Eq(Path::make(path), val));
}

// Match records which have a tag not equal to the specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::ne(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Ne(Path::make(path), val));
}

// Match records which have tags less than the specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::lt(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Lt(Path::make(path), val));
}

// Match records which have tags less than or equals to specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::le(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Le(Path::make(path), val));
}

// Match records which have tags greater than specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::gt(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Gt(Path::make(path), val));
}

// Match records which have tags greater than or equal to specified value.
// If the path is not defined then it is unmatched.
Filter::shared_ptr_t Filter::ge(const std::string& path, Val::auto_ptr_t val)
{
    return shared_ptr_t(new Ge(Path::make(path), val));
}

// Return a query which is the logical-and of this and that query.
Filter::shared_ptr_t Filter::AND(shared_ptr_t second)
{
    return shared_ptr_t(new And(shared_from_this(), second));
}

// Return a query which is the logical-or of this and that query.
Filter::shared_ptr_t Filter::OR(shared_ptr_t second)
{
    return shared_ptr_t(new Or(shared_from_this(), second));
}

std::string Filter::str() const
{
    return "";
}
bool Filter::operator ==(const Filter& other)
{
    return str() == other.str();
}

//////////////////////////////////////////////////////////////////////////
// Path
//////////////////////////////////////////////////////////////////////////

// Construct a new Path from string or throw exception
Path::auto_ptr_t Path::make(const std::string& path)
{

    // optimize for common single name case
    size_t dash = path.find('-');
    if (dash == path.npos)
        return auto_ptr_t(new Path1(path));

    // parse
    size_t s = 0;
    std::vector<std::string> acc;
    for (;;)
    {
        std::string n = path.substr(s, dash);
        if (n.size() == 0)
            throw std::runtime_error("Invalid path expr.");
        acc.push_back(n);
        if (path[dash + 1] != '>')
            throw std::runtime_error("Missing '>' on de-ref.");
        s = dash + 2;
        dash = path.find('-', s);
        if (dash == path.npos)
        {
            n = path.substr(s);
            if (n.size() == 0) throw std::runtime_error("Invalid path expr.");
            acc.push_back(n);
            break;
        }
    }
    return auto_ptr_t(new PathN(path, acc));
}

//////////////////////////////////////////////////////////////////////////
// Path1
//////////////////////////////////////////////////////////////////////////

Path1::Path1(std::string n) : m_name(n) {}
size_t Path1::size() const { return 1; }
std::string Path1::get(size_t i) const { if (i == 0) return m_name; throw std::runtime_error("index error"); }
std::string Path1::str() const { return m_name; }

//////////////////////////////////////////////////////////////////////////
// PathN
//////////////////////////////////////////////////////////////////////////

PathN::PathN(const std::string& s, const std::vector<std::string>& v) : m_string(s), m_names(v) {}
size_t PathN::size() const { return m_names.size(); }
std::string PathN::get(size_t i) const { return m_names[i]; }
std::string PathN::str() const { return m_string; }

//////////////////////////////////////////////////////////////////////////
// PathFilter
//////////////////////////////////////////////////////////////////////////
PathFilter::PathFilter(Path::auto_ptr_t p) : m_path(p) {}

bool PathFilter::include(const Dict& dict, const Pather& pather) const
{
    Val* val = (Val*)&dict.get(m_path->get(0), false);

    if (m_path->size() != 1)
    {
        for (size_t i = 1; i < m_path->size(); ++i)
        {
            if (val->type() != Val::REF_TYPE) { return do_include(EmptyVal::DEF); }
            Dict& nt = (Dict&)pather.find(((Ref&)*val).value);

            if (nt.size() == 0) { return do_include(EmptyVal::DEF); }
            val = (Val*)&nt.get(m_path->get(i), false);
        }
    }
    return do_include(*val);
}

std::string PathFilter::str() const
{
    return m_path->str();
}

const Path* PathFilter::path() const
{
    return m_path.get();
}

//////////////////////////////////////////////////////////////////////////
// Has
//////////////////////////////////////////////////////////////////////////
Has::Has(Path::auto_ptr_t p) : PathFilter(p) {}
bool Has::do_include(const Val& val) const { return !val.is_empty(); }
std::string Has::str() const { return PathFilter::str(); }

//////////////////////////////////////////////////////////////////////////
// Missing
//////////////////////////////////////////////////////////////////////////
Missing::Missing(Path::auto_ptr_t p) : PathFilter(p) {}
bool Missing::do_include(const Val& val) const { return val.is_empty(); }
std::string Missing::str() const
{
    std::stringstream ss;
    ss << "not" << PathFilter::str();
    return ss.str();
}

//////////////////////////////////////////////////////////////////////////
// CmpFilter
//////////////////////////////////////////////////////////////////////////
CmpFilter::CmpFilter(Path::auto_ptr_t p, Val::auto_ptr_t v) : PathFilter(p), m_val(v) {}
std::string CmpFilter::str() const
{
    std::stringstream ss;
    ss << path()->str() << cmp_str() << m_val->to_zinc();
    return ss.str();
}
const Val& CmpFilter::val() const { return *m_val; }
bool CmpFilter::same_type(const Val& v) const { return !v.is_empty() && v.type() == m_val->type(); }

//////////////////////////////////////////////////////////////////////////
// Eq
//////////////////////////////////////////////////////////////////////////
Eq::Eq(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Eq::cmp_str() const
{
    return "==";
}
bool Eq::do_include(const Val& val) const
{
    return !val.is_empty() && CmpFilter::val() == val;
}
//////////////////////////////////////////////////////////////////////////
// Ne
//////////////////////////////////////////////////////////////////////////
Ne::Ne(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Ne::cmp_str() const
{
    return "!=";
}
bool Ne::do_include(const Val& val) const
{
    return !val.is_empty() && !(CmpFilter::val() == val);
}

//////////////////////////////////////////////////////////////////////////
// Lt
//////////////////////////////////////////////////////////////////////////
Lt::Lt(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Lt::cmp_str() const
{
    return "<";
}
bool Lt::do_include(const Val& val) const
{
    return same_type(val) && val < CmpFilter::val();
}

//////////////////////////////////////////////////////////////////////////
// Le
//////////////////////////////////////////////////////////////////////////
Le::Le(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Le::cmp_str() const
{
    return "<=";
}
bool Le::do_include(const Val& val) const
{
    return same_type(val) && (CmpFilter::val() == val || val < CmpFilter::val());
}

//////////////////////////////////////////////////////////////////////////
// Gt
//////////////////////////////////////////////////////////////////////////
Gt::Gt(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Gt::cmp_str() const { return ">"; }
bool Gt::do_include(const Val& val) const { return same_type(val) && val > CmpFilter::val(); }

//////////////////////////////////////////////////////////////////////////
// Ge
//////////////////////////////////////////////////////////////////////////
Ge::Ge(Path::auto_ptr_t p, Val::auto_ptr_t v) : CmpFilter(p, v){}
std::string Ge::cmp_str() const
{
    return ">=";
}
bool Ge::do_include(const Val& val) const
{
    return same_type(val) && (CmpFilter::val() == val || val > CmpFilter::val());
}

//////////////////////////////////////////////////////////////////////////
// Compound
//////////////////////////////////////////////////////////////////////////
CompoundFilter::CompoundFilter(Filter::shared_ptr_t a, Filter::shared_ptr_t b) : m_a(a), m_b(b) {}

Filter::Type CompoundFilter::type() const { return COMPOUND_FILTER_TYPE; }

std::string CompoundFilter::str() const
{
    std::stringstream ss;

    if (m_a->type() == COMPOUND_FILTER_TYPE)
        ss << '(' << m_a->str() << ')';
    else
        ss << m_a->str();

    ss << ' ' << keyword() << ' ';

    if (m_b->type() == COMPOUND_FILTER_TYPE)
        ss << '(' << m_b->str() << ')';
    else
        ss << m_b->str();

    return ss.str();
}
const Filter& CompoundFilter::a() const { return *m_a; }
const Filter& CompoundFilter::b() const { return *m_b; }

//////////////////////////////////////////////////////////////////////////
// And
//////////////////////////////////////////////////////////////////////////
And::And(Filter::shared_ptr_t a, Filter::shared_ptr_t b) : CompoundFilter(a, b) {}

std::string And::keyword() const { return "and"; }

bool And::include(const Dict& dict, const Pather& pather) const
{
    return CompoundFilter::a().include(dict, pather) && CompoundFilter::b().include(dict, pather);
}

//////////////////////////////////////////////////////////////////////////
// Or
//////////////////////////////////////////////////////////////////////////
Or::Or(Filter::shared_ptr_t a, Filter::shared_ptr_t b) : CompoundFilter(a, b) {}

std::string Or::keyword() const { return "or"; }

bool Or::include(const Dict& dict, const Pather& pather) const
{
    return CompoundFilter::a().include(dict, pather) || CompoundFilter::b().include(dict, pather);
}
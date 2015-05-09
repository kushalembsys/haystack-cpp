#pragma once
#include "val.hpp"
#include <vector>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   02 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    class Pather;
    class Val;
    class Dict;

    /**
     Filter models a parsed tag query string.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>
     */
    class Filter : public boost::enable_shared_from_this < Filter >
    {
    public:
        enum Type
        {
            NORMAL_FILTER_TYPE = 'F',
            COMPOUND_FILTER_TYPE = 'C'
        };

        typedef boost::shared_ptr<Filter> shared_ptr_t;

        static shared_ptr_t make(const std::string&, bool checked = true);

        //////////////////////////////////////////////////////////////////////////
        // Factories
        //////////////////////////////////////////////////////////////////////////

        /**
        Match records which have the specified tag path defined.
        */
        static shared_ptr_t has(const std::string& path);

        /**
        Match records which do not define the specified tag path.
        */
        static shared_ptr_t missing(const std::string&  path);

        /**
        Match records which have a tag are equal to the specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t eq(const std::string& path, Val::auto_ptr_t val);

        /**
        Match records which have a tag not equal to the specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t ne(const std::string& path, Val::auto_ptr_t val);

        /**
        Match records which have tags less than the specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t lt(const std::string& path, Val::auto_ptr_t val);

        /**
        Match records which have tags less than or equals to specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t le(const std::string& path, Val::auto_ptr_t val);

        /**
        Match records which have tags greater than specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t gt(const std::string& path, Val::auto_ptr_t val);

        /**
        Match records which have tags greater than or equal to specified value.
        If the path is not defined then it is unmatched.
        */
        static shared_ptr_t ge(const std::string& path, Val::auto_ptr_t val);

        /**
        Return a query which is the logical-and of this and that query.
        */
        shared_ptr_t AND(shared_ptr_t second);
        /**
        Return a query which is the logical-or of this and that query.
        */
        shared_ptr_t OR(shared_ptr_t second);

        /**
        Return if given tags entity matches this query.
        */
        virtual bool include(const Dict& dict, const Pather& pather) const = 0;

        virtual std::string str() const;
        virtual Type type() const { return NORMAL_FILTER_TYPE; };

        virtual bool operator == (const Filter& other);

        virtual ~Filter(){}

    };

    //////////////////////////////////////////////////////////////////////////
    // Path
    //////////////////////////////////////////////////////////////////////////

    /**
    Pather is a callback interface used to resolve query paths.
    */
    class Pather
    {
        /**
        Given a HRef string identifier, resolve to an entity's
        Dict respresentation or ref is not found return null.
        */
    public:
        virtual const Dict& find(const std::string& ref) const = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    // Path
    //////////////////////////////////////////////////////////////////////////

    /**
    Path is a simple name or a complex path using the "->" separator
    */
    class Path
    {
    public:
        virtual ~Path(){};
        typedef std::auto_ptr<Path> auto_ptr_t;
        /**
        Construct a new Path from string or throw exception
        */
        static auto_ptr_t make(const std::string& path);

        /**
        Number of names in the path.
        */
        virtual size_t size() const = 0;

        /**
        Get name at given index.
        */
        virtual std::string get(size_t i) const = 0;

        /**
        Equality is based on string.
        */
        virtual  bool operator==(const Path& that) const { return str() == that.str(); }

        /**
        Get string encoding.
        */
        virtual std::string str() const = 0;
    };

    class Path1 : public Path
    {
        friend class Path;
        Path1(std::string n);
        size_t size() const;
        std::string get(size_t i) const;
        std::string str() const;
    private:
        std::string m_name;
    };

    class PathN : public Path
    {
        friend class Path;
        PathN(const std::string& s, const std::vector<std::string>&);
        size_t size() const;
        std::string get(size_t i) const;
        std::string str() const;
    private:
        const std::string m_string;
        const std::vector<std::string> m_names;
    };

    //////////////////////////////////////////////////////////////////////////
    // PathFilter
    //////////////////////////////////////////////////////////////////////////
    class PathFilter : public Filter
    {
    protected:
        PathFilter(Path::auto_ptr_t p);
        virtual ~PathFilter(){}
        bool include(const Dict& dict, const Pather& pather) const;
        virtual bool do_include(const Val& val) const = 0;
        std::string str() const;
        const Path* path() const;
        Path::auto_ptr_t m_path;
    };

    //////////////////////////////////////////////////////////////////////////
    // Has
    //////////////////////////////////////////////////////////////////////////
    class Has : public PathFilter
    {
        friend class Filter;
        Has(Path::auto_ptr_t p);
        bool do_include(const Val& val) const;
        std::string str() const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Missing
    //////////////////////////////////////////////////////////////////////////
    class Missing : public PathFilter
    {
        friend class Filter;
        Missing(Path::auto_ptr_t p);
        bool do_include(const Val& val) const;
        std::string str() const;
    };

    //////////////////////////////////////////////////////////////////////////
    // CmpFilter
    //////////////////////////////////////////////////////////////////////////
    class CmpFilter : public PathFilter
    {
    protected:
        CmpFilter(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string str() const;
        bool same_type(const Val& v) const;
        virtual std::string cmp_str() const = 0;
        const Val& val() const;
        Val::auto_ptr_t m_val;
    };

    //////////////////////////////////////////////////////////////////////////
    // Eq
    //////////////////////////////////////////////////////////////////////////
    class Eq : public CmpFilter
    {
        friend class Filter;
        Eq(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Ne
    //////////////////////////////////////////////////////////////////////////
    class Ne : public CmpFilter
    {
        friend class Filter;
        Ne(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Lt
    //////////////////////////////////////////////////////////////////////////
    class Lt : public CmpFilter
    {
        friend class Filter;
        Lt(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Le
    //////////////////////////////////////////////////////////////////////////
    class Le : public CmpFilter
    {
        friend class Filter;
        Le(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Gt
    //////////////////////////////////////////////////////////////////////////
    class Gt : public CmpFilter
    {
        friend class Filter;
        Gt(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Ge
    //////////////////////////////////////////////////////////////////////////
    class Ge : public CmpFilter
    {
        friend class Filter;
        Ge(Path::auto_ptr_t, Val::auto_ptr_t v);
        std::string cmp_str() const;
        bool do_include(const Val& val) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // Compound
    //////////////////////////////////////////////////////////////////////////
    class CompoundFilter : public Filter
    {
    protected:

        CompoundFilter(Filter::shared_ptr_t a, Filter::shared_ptr_t b);
        virtual ~CompoundFilter(){}
        virtual std::string keyword() const = 0;
        Type type() const;
        std::string str() const;

        const Filter& a() const;
        const Filter& b() const;

        Filter::shared_ptr_t m_a;
        Filter::shared_ptr_t m_b;
    };

    //////////////////////////////////////////////////////////////////////////
    // And
    //////////////////////////////////////////////////////////////////////////
    class And : public CompoundFilter
    {
        friend class Filter;
        And(Filter::shared_ptr_t a, Filter::shared_ptr_t b);
        std::string keyword() const;
        bool include(const Dict& dict, const Pather& pather) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // And
    //////////////////////////////////////////////////////////////////////////
    class Or : public CompoundFilter
    {
        friend class Filter;
        Or(Filter::shared_ptr_t a, Filter::shared_ptr_t b);
        std::string keyword() const;
        bool include(const Dict& dict, const Pather& pather) const;
    };

};
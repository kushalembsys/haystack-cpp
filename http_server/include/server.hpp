#pragma once
#include "headers.hpp"
#include "dict.hpp"
#include "datetime.hpp"
#include "proj.hpp"

namespace haystack
{
    class Op;

    class const_proj_iterator
        : public boost::iterator_facade <
        const_proj_iterator
        , Dict const
        , boost::forward_traversal_tag
        >
    {
        public:
        friend class boost::iterator_core_access;

        virtual void increment() {};

        virtual bool equal(const_proj_iterator const& other) const { return false; }

        virtual const Dict& dereference() const { return Dict::EMPTY; }
    };

    // Server is the interface between HTTPRequestHandler and a database of
    // tag based entities.  All methods on HServer must be thread safe.
    //
    // @see <a href='http://project-haystack.org/doc/Rest'>Project Haystack</a>
    class Server : public Proj
    {

    public:

        typedef const_proj_iterator iterator;
        typedef const_proj_iterator const_iterator;

        Dict::auto_ptr_t about() const;
        //////////////////////////////////////////////////////////////////////////
        // Operations
        //////////////////////////////////////////////////////////////////////////

        // Return the operations supported by this database.
        virtual const std::vector<const Op*>& ops() = 0;

    protected:
        // Implementation hook for "about" method.
        // Should return these tags:
        //   - serverName: Str
        //   - productName: Str
        //   - productVersion: Str
        //   - productUri: Uri
        //   - moduleName: Str
        //   - moduleVersion: Str
        //  - moduleUri: Uri
        virtual const Dict& on_about() const = 0;

        Grid::auto_ptr_t on_read_by_ids(const std::vector<Ref>& ids);

        Grid::auto_ptr_t on_read_all(const std::string& filter, size_t limit);

        virtual const_iterator begin() = 0;
        virtual const_iterator end() = 0;

        static const DateTime& boot_time();

    private:
        static const DateTime* m_boot_time;
    };
};
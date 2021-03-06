#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "headers.hpp"
#include "dict.hpp"
#include "grid.hpp"
#include "ref.hpp"
#include "row.hpp"
#include <vector>
#include <limits>

namespace haystack {
    /**
    Proj is the common interface for Client and Server to provide
    access to a database tagged entity records.
    @see <a href='http://project-haystack.org/doc/TagModel'>Project Haystack</a>
    */
    class Proj
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // Operations
        //////////////////////////////////////////////////////////////////////////

        /**
        Get the summary "about" information.
        */
        virtual Dict::auto_ptr_t about() const = 0;

        //////////////////////////////////////////////////////////////////////////
        // Read by id
        //////////////////////////////////////////////////////////////////////////

        /**
        Convenience for "read_by_id(id, true)"
        */
        virtual Dict::auto_ptr_t read_by_id(const Ref& id) const;

        /**
        Call "read" to lookup an entity record by it's unique identifier.
        If not found then return null or throw an UnknownRecException based
        on checked.
        */
        virtual Dict::auto_ptr_t read_by_id(const Ref& id, bool checked) const;

        /**
        Convenience for "read_by_ids(ids, true)"
        */
        virtual Grid::auto_ptr_t read_by_ids(const boost::ptr_vector<Ref>& ids) const;

        /**
        Read a list of entity records by their unique identifier.
        Return a grid where each row of the grid maps to the respective
        id array (indexes line up).  If checked is true and any one of the
        ids cannot be resolved then raise exception for first id
        not resolved.  If checked is false, then each id not found has a
        row where every cell is null.
        */
        virtual Grid::auto_ptr_t read_by_ids(const boost::ptr_vector<Ref>& ids, bool checked) const;

        /**
        Subclass hook for read_by_id, return null if not found.
        */
        virtual Dict::auto_ptr_t on_read_by_id(const Ref& id) const = 0;

        /**
        Subclass hook for read_by_ids, return rows with nulls cells
        for each id not found.
        */
        virtual Grid::auto_ptr_t on_read_by_ids(const boost::ptr_vector<Ref>& ids) const = 0;

        //////////////////////////////////////////////////////////////////////////
        // Read by filter
        //////////////////////////////////////////////////////////////////////////

        /**
        Convenience for "read(filter, true)".
        */
        virtual Dict::auto_ptr_t read(const std::string& filter) const;

        /**
        Query one entity record that matches the given filter.  If
        there is more than one record, then it is undefined which one is
        returned.  If there are no matches than return null or raise
        runtime_exception based on checked flag.
        */
        virtual Dict::auto_ptr_t read(const std::string& filter, bool checked) const;

        /**
        * Convenience for "read_all(filter, max)".
        */
        virtual Grid::auto_ptr_t read_all(const std::string& filter) const;

        /**
        Call "read" to query every entity record that matches given filter.
        Clip number of results by "limit" parameter.
        */
        virtual Grid::auto_ptr_t read_all(const std::string& filter, size_t limit) const;

        /**
        Subclass hook for read and read_all.
        */
        virtual Grid::auto_ptr_t on_read_all(const std::string& filter, size_t limit) const = 0;

    };

}
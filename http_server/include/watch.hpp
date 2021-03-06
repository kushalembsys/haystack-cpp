#pragma once
//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// Licensed under the Academic Free License version 3.0
// History:
//   12 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

//#include "grid.hpp"
#include <boost/shared_ptr.hpp>

namespace haystack {
    class Grid;
    /**
    Watch models a subscription to a list of entity records.
    Use Proj::watch_open to create a new watch.
    @see <a href = 'http://project-haystack.org/doc/Rest#watches'>Project Haystack< / a>
    */
    class Watch : boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<Watch> shared_ptr;
        typedef boost::ptr_vector<Ref> refs_t;
        /**
        Unique watch identifier within a project database.
        The id may not be assigned until after the first call
        to "sub", in which case return "".
        */
        virtual const std::string id() const = 0;
        /**
        Debug display string used during "Proj.watch_open"
        */
        virtual const std::string dis() const = 0;

        /**
        Lease period or int::min if watch has not been opened yet.
        */
        virtual const int lease() const = 0;

        /**
        Add a list of records to the subscription list and return their
        current representation.  If checked is true and any one of the
        ids cannot be resolved then raise runtime_exception for first id
        not resolved.  If checked is false, then each id not found has a
        row where every cell is null.
        The Grid that is returned must contain metadata entries
        for 'watchId' and 'lease'.
        */
        virtual Grid::auto_ptr_t sub(const refs_t& ids, bool checked = true) = 0;

        /**
        Remove a list of records from watch.  Silently ignore
        any invalid ids.
        */
        virtual void unsub(const refs_t& ids) = 0;

        /**
        Poll for any changes to the subscribed records.
        */
        virtual Grid::auto_ptr_t poll_changes() = 0;

        /**
        Poll all the subscribed records even if there have been no changes.
        */
        virtual Grid::auto_ptr_t poll_refresh() = 0;

        /**
        Close the watch and free up any state resources.
        */
        virtual void close() = 0;

        /**
        Return whether this watch is currently open.
        */
        virtual bool is_open() const = 0;

    };
}
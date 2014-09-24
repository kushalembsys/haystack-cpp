#pragma once
#include "headers.hpp"
#include "dict.hpp"
#include "datetime.hpp"
#include "proj.hpp"
#include "watch.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   08 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack
{
    class Num;
    class Op;
    class DateTimeRange;
    class HisItem;
    class Uri;

    class const_proj_iterator
        : public boost::iterator_facade <
        const_proj_iterator
        , Dict const
        , boost::forward_traversal_tag
        >
    {
    public:
        friend class boost::iterator_core_access;

        const_proj_iterator(boost::ptr_map<std::string, Dict>::const_iterator it) : m_it(it){}

        void increment() { ++m_it; }

        bool equal(const_proj_iterator const& other) const { return m_it == other.m_it; }

        const Dict& dereference() const
        {
            return *m_it->second;
        }
    private:
        boost::ptr_map<std::string, Dict>::const_iterator m_it;
    };

    /**
    Server is the interface between HTTPRequestHandler and a database of
    tag based entities.  All methods on HServer must be thread safe.
    @see <a href='http://project-haystack.org/doc/Rest'>Project Haystack</a>
    */
    class Server : public Proj
    {

    public:

        typedef const_proj_iterator iterator;
        typedef const_proj_iterator const_iterator;

        Server() { boot_time(); }

        Dict::auto_ptr_t about() const;
        //////////////////////////////////////////////////////////////////////////
        // Operations
        //////////////////////////////////////////////////////////////////////////

        /**
        Return the operations supported by this database.
        */
        virtual const std::vector<const Op*>& ops() = 0;

        //////////////////////////////////////////////////////////////////////////
        // Navigation
        //////////////////////////////////////////////////////////////////////////

        /**
        Return navigation children for given navId.
        */
        Grid::auto_ptr_t nav(const std::string& navId) const;

        /**
        Read a record from the database using a navigation path.
        If not found then return NULL or raise runtime_exception
        base on checked flag.
        */
        Dict::auto_ptr_t nav_read_by_uri(const Uri& uri, bool checked) const;

    protected:
        //
        // Implementation hook for "about" method.
        // Should return these tags:
        //   - serverName: Str
        //   - productName: Str
        //   - productVersion: Str
        //   - productUri: Uri
        //   - moduleName: Str
        //   - moduleVersion: Str
        //  - moduleUri: Uri
        //
        virtual const Dict& on_about() const = 0;

        Grid::auto_ptr_t on_read_by_ids(const boost::ptr_vector<Ref>& ids) const;

        Grid::auto_ptr_t on_read_all(const std::string& filter, size_t limit) const;

        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const = 0;

        //
        // Return navigation tree children for given navId.
        // The grid must define the "navId" column.
        //
        virtual Grid::auto_ptr_t on_nav(const std::string& nav_id) const = 0;
        //
        // Implementation hook for nav_read_by_uri.  Return null if not
        // found.  Do NOT raise any exceptions.
        //
        virtual Dict::auto_ptr_t on_nav_read_by_uri(const Uri& uri) const = 0;

    public:
        //////////////////////////////////////////////////////////////////////////
        // Watches
        //////////////////////////////////////////////////////////////////////////
        //
        // Create a new watch with an empty subscriber list.  The dis
        // string is a debug string to keep track of who created the watch.
        //
        const Watch::shared_ptr watch_open(const std::string& dis);

        /**
        List the open watches.
        */
        const std::vector<Watch::shared_ptr> watches();
        //
        // Lookup a watch by its unique identifier.  If not found then
        // raise runtime_error or return NULL based on checked flag.
        //
        virtual Watch::shared_ptr watch(const std::string& id, bool checked = true);
    protected:
        /**
        Implementation hook for on_watch_open.
        */
        virtual Watch::shared_ptr on_watch_open(const std::string& dis) = 0;

        /**
        Implementation hook for watches.
        */
        virtual const std::vector<Watch::shared_ptr> on_watches() = 0;

        /**
        Implementation hook for watch lookup, return null if not found.
        */
        virtual  Watch::shared_ptr on_watch(const std::string& id) = 0;
    public:
        //////////////////////////////////////////////////////////////////////////
        // Point Writes
        //////////////////////////////////////////////////////////////////////////
        //
        // Return priority array for writable point identified by id.
        // The grid contains 17 rows with following columns:
        //   - level: number from 1 - 17 (17 is default)
        //   - levelDis: human description of level
        //   - val: current value at level or null
        //   - who: who last controlled the value at this level
        //
        Grid::auto_ptr_t point_write_array(const Ref& id);

        /**
        Write to the given priority array level.
        */
        void point_write(const Ref& id, int level, const Val& val, const std::string& who, const Num& dur);
    protected:
        /**
        Implementation hook for pointWriteArray
        */
        virtual Grid::auto_ptr_t on_point_write_array(const Dict& rec) = 0;

        /**
        Implementation hook for pointWrite
        */
        virtual void on_point_write(const Dict& rec, int level, const Val& val, const std::string& who, const Num& dur) = 0;

    public:
        //////////////////////////////////////////////////////////////////////////
        // History
        //////////////////////////////////////////////////////////////////////////
        //
        // Read history time-series data for given record and time range. The
        // items returned are exclusive of start time and inclusive of end time.
        // Raise exception if id does not map to a record with the required tags
        // "his" or "tz".  The range may be either a StrRange or a DateTimeRange.
        // If DateTimeRange is passed then must match the timezone configured on
        // the history record.  Otherwise if a StrRange is passed, it is resolved
        // relative to the history record's timezone.
        //
        Grid::auto_ptr_t his_read(const Ref& id, const std::string& range);
        //
        // Write a set of history time-series data to the given point record.
        // The record must already be defined and must be properly tagged as
        // a historized point.  The timestamp timezone must exactly match the
        // point's configured "tz" tag.  If duplicate or out-of-order items are
        // inserted then they must be gracefully merged.
        //
        void his_write(const Ref& id, const std::vector<HisItem>& items);

    protected:
        //
        // Implementation hook for hisRead.  The items must be exclusive
        // of start and inclusive of end time.
        //
        virtual std::vector<HisItem> on_his_read(const Dict& rec, const DateTimeRange& range) = 0;

        /**
        Implementation hook for onHisWrite.
        */
        virtual void on_his_write(const Dict& rec, const std::vector<HisItem>& items) = 0;

    public:
        //////////////////////////////////////////////////////////////////////////
        // Actions
        //////////////////////////////////////////////////////////////////////////

        /**
        Invoke an action identified by id and action.
        */
        Grid::auto_ptr_t invoke_action(const Ref& id, const std::string& action, const Dict& args)
        {
            // lookup entity
            Dict::auto_ptr_t rec = read_by_id(id);

            // route to subclass
            return on_invoke_action(*rec, action, args);
        }

     protected:
         /**
         Implementation hook for invokeAction
         */
         virtual Grid::auto_ptr_t on_invoke_action(const Dict& rec, const std::string& action, const Dict& args) = 0;

    public:
        // Impl

        static const DateTime& boot_time();

    private:

        static const DateTime* m_boot_time;
    };
};
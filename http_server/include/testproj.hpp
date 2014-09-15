#pragma once
#include "server.hpp"

//
// Copyright (c) 2014, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   09 Sep 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack
{
    class Dict;
    
    // TestProj provides a simple implementation of
    // Server with some test entities.
    class TestProj : public Server
    {

    public:
        typedef boost::ptr_map<std::string, Dict> recs_t;
        
        TestProj();
        //////////////////////////////////////////////////////////////////////////
        // Ops
        //////////////////////////////////////////////////////////////////////////
        const std::vector<const Op*>& ops();
        const Op* const op(const std::string& name, bool checked = true) const;
        const Dict& on_about() const;
    protected:
        //////////////////////////////////////////////////////////////////////////
        // Reads
        //////////////////////////////////////////////////////////////////////////
        
        Dict::auto_ptr_t on_read_by_id(const Ref& id) const;
       
        //////////////////////////////////////////////////////////////////////////
        // Navigation
        //////////////////////////////////////////////////////////////////////////

        Grid::auto_ptr_t on_nav(const std::string& nav_id) const;

        Dict::auto_ptr_t on_nav_read_by_uri(const Uri& uri) const;

        //////////////////////////////////////////////////////////////////////////
        // Watches
        //////////////////////////////////////////////////////////////////////////

        Watch::shared_ptr on_watch_open(const std::string& dis);
        
        const std::vector<Watch::shared_ptr> on_watches();

        Watch::shared_ptr on_watch(const std::string& id);
        
        //////////////////////////////////////////////////////////////////////////
        // Point Write
        //////////////////////////////////////////////////////////////////////////

        Grid::auto_ptr_t on_point_write_array(const Dict& rec);

        void on_point_write(const Dict& rec, int level, const Val& val, const std::string& who, const Num& dur);
        
        //////////////////////////////////////////////////////////////////////////
        // History
        //////////////////////////////////////////////////////////////////////////

        std::vector<HisItem> on_his_read(const Dict& entity, const DateTimeRange& range);
        void on_his_write(const Dict& rec, const std::vector<HisItem>& items);
        
        //////////////////////////////////////////////////////////////////////////
        // Actions
        //////////////////////////////////////////////////////////////////////////
        
        Grid::auto_ptr_t on_invoke_action(const Dict& rec, const std::string& action, const Dict& args);
    
    public:


        const_iterator begin() const;
        const_iterator end() const;
    private:
        void add_site(const std::string& dis, const std::string& geoCity, const std::string& geoState, int area);
        void add_meter(Dict& site, const std::string& dis);
        void add_ahu(Dict& site, const std::string& dis);
        void add_point(Dict& equip, const std::string& dis, const std::string& unit, const std::string& markers);

        recs_t m_recs;
        
        static Dict* m_about;
        static std::vector<const Op*>* m_ops;
    };
}
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
        //////////////////////////////////////////////////////////////////////////
        // Reads
        //////////////////////////////////////////////////////////////////////////
        Dict::auto_ptr_t on_read_by_id(const Ref& id) const;

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
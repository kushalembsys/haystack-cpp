#include "server.hpp"
#include "filter.hpp"

using namespace haystack;

Dict::auto_ptr_t Server::about() const
{
    Dict::auto_ptr_t  d(new Dict());
    d->add(on_about())
        .add("haystackVersion", "2.0")
        .add("serverTime", DateTime::now())
        .add("serverBootTime", boot_time())
        .add("tz", TimeZone::DEFAULT.name);
    return d;
}

Grid::auto_ptr_t Server::on_read_by_ids(const std::vector<Ref>& ids) const
{
    boost::ptr_vector<Dict> v;

    for (std::vector<Ref>::const_iterator it = ids.begin(), end = ids.end(); it != end; ++it)
    {
        v.push_back(on_read_by_id(*it).release());
    }
    return Grid::make(v);
}

class PathImpl : public Pather
{
 public:
    PathImpl(const Server& s) : m_s(s) {}
    const Dict& find(const std::string& ref) const
    {
        Server& s = const_cast<Server&>(m_s);
        const_cast<PathImpl*>(this)->m_d = s.read_by_id(Ref(ref));
        return *m_d;
    }
private:
    const Server& m_s;
    Dict::auto_ptr_t m_d;
};

Grid::auto_ptr_t Server::on_read_all(const std::string& filter, size_t limit) const
{
    Filter::auto_ptr_t f = Filter::make(filter);
    PathImpl pather(*this);
    
   // boost::ptr_vector<Dict> v;
    std::vector<const Dict* const> v2;

    for (const_iterator it = begin(), e = end(); it != e; ++it)
    {
        const Dict& row = *it;
        if (row.is_empty())
            continue;

        if (f->include(row, pather))
        {
            //v.push_back((Dict*)&row);
            v2.push_back(&*it);
            if (v2.size() > limit)
                break;
        }
    }

    return Grid::make(v2);
}

const DateTime& Server::boot_time()
{
    if (m_boot_time != NULL)
        return *m_boot_time;

    m_boot_time = (DateTime*) DateTime::now().clone().release();
    return *m_boot_time;
}

const DateTime* Server::m_boot_time = NULL;

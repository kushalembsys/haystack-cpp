#include "server.hpp"
#include "hisitem.hpp"
#include "filter.hpp"
#include "uri.hpp"
#include "datetimerange.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>

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

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

// Return the operations supported by this database.
Grid::auto_ptr_t Server::on_read_by_ids(const std::vector<Ref>& ids) const
{
    boost::ptr_vector<Dict> v(ids.size());

    for (std::vector<Ref>::const_iterator it = ids.begin(), end = ids.end(); it != end; ++it)
    {
        v.push_back(on_read_by_id(*it).release());
    }
    return Grid::make(v);
}

//////////////////////////////////////////////////////////////////////////
// Navigation
//////////////////////////////////////////////////////////////////////////

// Return navigation children for given navId.
Grid::auto_ptr_t Server::nav(const std::string& navId) const
{
    return on_nav(navId);
}

// Read a record from the database using a navigation path.
// If not found then return NULL or raise runtime_exception
// base on checked flag.
Dict::auto_ptr_t Server::nav_read_by_uri(const Uri& uri, bool checked) const
{
    Dict::auto_ptr_t rec = on_nav_read_by_uri(uri);
    if (rec.get() != NULL) return rec;
    if (checked) throw std::runtime_error(uri.to_string());
    return Dict::auto_ptr_t();
}
//////////////////////////////////////////////////////////////////////////
// Watches
//////////////////////////////////////////////////////////////////////////

// Create a new watch with an empty subscriber list.  The dis
// string is a debug string to keep track of who created the watch.
const Watch::shared_ptr Server::watch_open(const std::string& dis)
{
    std::string watch = dis;
    boost::algorithm::trim(watch);
    if (watch.size() == 0) throw std::runtime_error("dis is empty");
    return on_watch_open(watch);
}
// List the open watches.
const std::vector<Watch::shared_ptr> Server::watches()
{
    return on_watches();
}

// Lookup a watch by its unique identifier.  If not found then
// raise runtime_error or return NULL based on checked flag.
Watch::shared_ptr Server::watch(const std::string& id, bool checked/* = true*/)
{
    Watch::shared_ptr w = on_watch(id);
    if (w.get() != NULL) return w;
    if (checked) throw std::runtime_error(id);
    return Watch::shared_ptr();
}

//////////////////////////////////////////////////////////////////////////
// Point Writes
//////////////////////////////////////////////////////////////////////////

// Return priority array for writable point identified by id.
// The grid contains 17 rows with following columns:
//   - level: number from 1 - 17 (17 is default)
//   - levelDis: human description of level
//   - val: current value at level or null
//   - who: who last controlled the value at this level
Grid::auto_ptr_t Server::point_write_array(const Ref& id)
{
    // lookup entity
    Dict::auto_ptr_t rec = read_by_id(id);

    // check that entity has "writable" tag
    if (rec->missing("writable"))
        throw std::runtime_error("Rec missing 'writable' tag: " + rec->dis());

    return on_point_write_array(*rec);
}

// Write to the given priority array level.
void Server::point_write(const Ref& id, int level, const Val& val, const std::string& who, const Num& dur)
{
    // argument checks
    if (level < 1 || level > 17) throw std::runtime_error("Invalid level 1-17: " + level);
    if (who.empty()) throw std::runtime_error("who is empty ''");

    // lookup entity
    Dict::auto_ptr_t rec = read_by_id(id);

    // check that entity has "writable" tag
    if (rec->missing("writable"))
        throw std::runtime_error("Rec missing 'writable' tag: " + rec->dis());

    on_point_write(*rec, level, val, who, dur);
}

//////////////////////////////////////////////////////////////////////////
// History
//////////////////////////////////////////////////////////////////////////

Grid::auto_ptr_t Server::his_read(const Ref& id, const std::string& range)
{
    // lookup entity
    Dict::auto_ptr_t rec = read_by_id(id);

    // check that entity has "his" tag
    if (rec->missing("his"))
        throw std::runtime_error("Rec missing 'his' tag: " + rec->dis());

    // lookup "tz" on entity
    boost::scoped_ptr<TimeZone> tz;
    if (rec->has("tz")) tz.reset(new TimeZone(rec->get_str("tz"), false));
    if (tz.get() == NULL)
        throw std::runtime_error("Rec missing or invalid 'tz' tag: " + rec->dis());

    // check or parse date range
    DateTimeRange::auto_ptr_t r;
    try
    {
        r = DateTimeRange::make(range, *tz);
    }
    catch (std::exception&)
    {
        throw std::runtime_error("Invalid date time range: " + range);
    }

    // checking
    if (!(r->start().tz == *tz))
        throw std::runtime_error("range.tz != rec: " + r->start().tz.name + " != " + tz->name);

    // route to subclass
    std::vector<HisItem> items = on_his_read(*rec, *r);

    // check items
    if (items.size() > 0)
    {
        if (r->start().millis() >= items[0].ts->millis()) throw std::runtime_error("start range not met");
        if (r->end().millis() < items[items.size() - 1].ts->millis()) std::runtime_error("end range not met");
    }

    // build and return result grid
    Dict meta;
    meta.add("id", id)
        .add("hisStart", r->start())
        .add("hisEnd", r->end());
    return HisItem::his_items_to_grid(meta, items);
}

void Server::his_write(const Ref& id, const std::vector<HisItem>& items)
{
    // lookup entity
    Dict::auto_ptr_t rec = read_by_id(id);

    // check that entity has "his" tag
    if (rec->missing("his"))
        throw std::runtime_error("Entity missing 'his' tag: " + rec->dis());

    // lookup "tz" on entity
    boost::scoped_ptr<TimeZone> tz;
    if (rec->has("tz")) tz.reset(new TimeZone(rec->get_str("tz"), false));
    if (tz.get() == NULL)
        throw std::runtime_error("Rec missing or invalid 'tz' tag: " + rec->dis());

    // check tz of items
    if (items.size() == 0) return;

    for (std::vector<HisItem>::const_iterator it = items.begin(), e = items.end(); it != e; ++it)
    {
        if (!(it->ts->tz == *tz)) 
            throw std::runtime_error("item.tz != rec.tz: " + it->ts->tz.name + " != " + tz->name);
    }

    // route to subclass
    on_his_write(*rec, items);
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
    Filter::shared_ptr_t f = Filter::make(filter);
    PathImpl pather(*this);

    std::vector<const Dict*> v;

    for (const_iterator it = begin(), e = end(); it != e; ++it)
    {
        const Dict& row = *it;
        if (row.is_empty())
            continue;

        if (f->include(row, pather))
        {
            v.push_back(&*it);
            if (v.size() > limit)
                break;
        }
    }

    return Grid::make(v);
}

const DateTime& Server::boot_time()
{
    if (m_boot_time != NULL)
        return *m_boot_time;

    m_boot_time = (DateTime*)DateTime::now().clone().release();
    return *m_boot_time;
}

const DateTime* Server::m_boot_time = NULL;

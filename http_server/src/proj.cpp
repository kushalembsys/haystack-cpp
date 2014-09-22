#include "proj.hpp"

using namespace haystack;

Dict::auto_ptr_t Proj::read_by_id(const Ref& id) const
{
    return read_by_id(id, true);
}

Dict::auto_ptr_t Proj::read_by_id(const Ref& id, bool checked) const
{
    Dict::auto_ptr_t rec = on_read_by_id(id);
    if (rec.get() != NULL) return rec;
    if (checked) throw std::runtime_error("Unknown id: " + id.to_string());
    return rec;
}

Grid::auto_ptr_t Proj::read_by_ids(const boost::ptr_vector<Ref>& ids) const
{
    return read_by_ids(ids, true);
}

Grid::auto_ptr_t Proj::read_by_ids(const boost::ptr_vector<Ref>& ids, bool checked) const
{
    Grid::auto_ptr_t grid = on_read_by_ids(ids);
    if (checked)
    {
        for (Grid::const_iterator it = grid->begin(), end = grid->end(); it != end; ++it)
            if (it->missing("id")) throw std::runtime_error("Unknown id: " + it->to_string());
    }
    return grid;
}

Dict::auto_ptr_t Proj::read(const std::string& filter) const
{
    return read(filter, true);
}

Dict::auto_ptr_t Proj::read(const std::string& filter, bool checked) const
{
    Grid::auto_ptr_t grid = read_all(filter, 1);
    if (grid->num_rows() > 0) return grid->row(0).to_dict();
    if (checked) throw std::runtime_error(filter);
    return Dict::auto_ptr_t();
}

Grid::auto_ptr_t Proj::read_all(const std::string& filter) const
{
    return read_all(filter, (std::numeric_limits<std::size_t>::max()));
}

Grid::auto_ptr_t Proj::read_all(const std::string& filter, size_t limit) const
{
    return on_read_all(filter, limit);
}
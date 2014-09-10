#include "proj.hpp"

using namespace haystack;

Dict::auto_ptr_t Proj::read_by_id(const Ref& id)
{
    return read_by_id(id, true);
}

Dict::auto_ptr_t Proj::read_by_id(const Ref& id, bool checked)
{
    Dict::auto_ptr_t rec = on_read_by_id(id);
    if (!rec->is_empty()) return rec;
    if (checked) throw std::runtime_error("Unknown id: " + id.to_string());
    return Dict::auto_ptr_t();
}

Grid::auto_ptr_t Proj::read_by_ids(const std::vector<Ref>& ids)
{
    return read_by_ids(ids, true);
}

Grid::auto_ptr_t Proj::read_by_ids(const std::vector<Ref>& ids, bool checked)
{
    Grid::auto_ptr_t grid = on_read_by_ids(ids);
    if (checked)
    {
        for (Grid::const_iterator it = grid->begin(), end = grid->end(); it != end; ++it)
            if (it->missing("id")) throw std::runtime_error("Unknown id: " + it->to_string());
    }
    return grid;
}

Dict::auto_ptr_t Proj::read(const std::string& filter)
{
    return read(filter, true);
}

Dict::auto_ptr_t Proj::read(const std::string& filter, bool checked)
{
    Grid::auto_ptr_t grid = read_all(filter, 1);
    if (grid->num_rows() > 0) return ((Dict&)grid->row(0)).clone();
    if (checked) throw std::runtime_error(filter);
    return Dict::auto_ptr_t();
}

Grid::auto_ptr_t Proj::read_all(const std::string& filter)
{
    return read_all(filter, (std::numeric_limits<std::size_t>::max()));
}

Grid::auto_ptr_t Proj::read_all(const std::string& filter, size_t limit)
{
    return on_read_all(filter, limit);
}
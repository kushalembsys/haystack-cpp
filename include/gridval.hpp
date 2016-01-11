#pragma once
//
// Copyright (c) 2016, Siemens Building Technolgies
// History:
//   06 Jan 2016  Steve Fritsche, adapted from Bool.hpp
//

#include "val.hpp"
#include "grid.hpp"

namespace haystack {

    class GridVal : public Val
    {
        // disable assignment
        GridVal& operator = (const GridVal &other);
        GridVal(const GridVal &other){};
    public:
        const Type type() const { return GRID_TYPE; }

        /**
        This grid value
        */
        Grid m_grid;

        GridVal();

       const std::string to_string() const;

       const std::string to_zinc() const;

        /**
        Equality
        */
        bool operator == (const GridVal &other) const;
        bool operator == (const Val &other) const;

        bool operator > (const Val &other) const;
        bool operator < (const Val &other) const;

        auto_ptr_t clone() const;
    };
};

#pragma once
#include "gridreader.hpp"
#include "val.hpp"
#include "filter.hpp"
#include <istream>
#include <stdint.h>

//
// Copyright (c) 2015, J2 Innovations
// Copyright (c) 2012 Brian Frank
// History:
//   29 Aug 2014  Radu Racariu<radur@2inn.com> Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

namespace haystack {
    class Dict;
    class Filter;
    /**
     ZincReader reads grids using the Zinc format.

     @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a>

     */
    class ZincReader : public GridReader
    {
    public:

        ZincReader(std::istream& is);
        virtual ~ZincReader(){}
        static std::auto_ptr<ZincReader> make(const std::string& s);

        /**
        Read a grid
        */
        std::auto_ptr<Grid> read_grid();
        /**
        Parses a filter
        */
        Filter::shared_ptr_t read_filter();
        /**
        Read set of name/value tags as dictionary
        */
        std::auto_ptr<Dict> read_dict();
        /**
        Read scalar value.
        */
        Val::auto_ptr_t read_scalar();

    private:
        // private ctor for owning a local input stream
        ZincReader(std::auto_ptr<std::istream>);
        //////////////////////////////////////////////////////////////////////////
        // Implementation
        //////////////////////////////////////////////////////////////////////////
        void read_ver();
        void read_meta(Dict&);

        // no ownership

        // Read a single scalar value from the stream.
        Val::auto_ptr_t read_val();
        Val::auto_ptr_t read_num_val();
        Val::auto_ptr_t read_bin_val();
        Val::auto_ptr_t read_coord_val();
        Val::auto_ptr_t read_ref_val();
        Val::auto_ptr_t read_str_val();
        Val::auto_ptr_t read_uri_val();
        Val::auto_ptr_t read_word_val();

        Filter::shared_ptr_t read_filter_or();
        Filter::shared_ptr_t read_filter_and();
        Filter::shared_ptr_t read_filter_atomic();
        Filter::shared_ptr_t read_filter_parens();

        void consume_cmp();
        std::string read_filter_path();

        std::string read_id();
        std::string read_str_literal();
        int32_t read_two_digits(std::string errMsg);
        int32_t read_esc_char();

        static const char *charType;
        // static init the charType table
        static char* init_table();
        static bool is_id(int32_t);
        static bool is_id_start(int32_t);
        static bool is_tz(int32_t);
        static bool is_unit(int32_t);
        static bool is_digit(int32_t);
        static bool is_alpha(int32_t);

        int32_t to_nibble(int32_t c);
        void skip_space();
        void consume_new_line();
        void consume();

        //////////////////////////////////////////////////////////////////////////
        // Fields
        //////////////////////////////////////////////////////////////////////////

        std::istream& m_is;
        std::auto_ptr<std::istream> m_local_is;
        int32_t m_cur;
        int32_t m_peek;
        int32_t m_line_num;
        int32_t m_version;
        bool m_is_filter;
    };
};
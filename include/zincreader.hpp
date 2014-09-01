#pragma once
#include "gridreader.hpp"
#include <istream>
#include <stdint.h>

//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   29 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//

/**
 ZincReader reads grids using the Zinc format.

 @see <a href='http://project-haystack.org/doc/TagModel#tagKinds'>Project Haystack</a> 

*/
namespace haystack {
class Dict;
class ZincReader : public GridReader
{
public:

    ZincReader(std::istream& is);

    // Read a grid
    const Grid& read_grid();

private:
    //////////////////////////////////////////////////////////////////////////
    // Fields
    //////////////////////////////////////////////////////////////////////////

    std::istream& m_is;
    int32_t m_cur;
    int32_t m_peek;
    int32_t m_line_num;
    int32_t m_version;
    bool m_is_filter;

    //////////////////////////////////////////////////////////////////////////
    // Implementation
    //////////////////////////////////////////////////////////////////////////
    void read_ver();
    void read_meta(Dict&);

    const Val& read_val();
    const Val& read_scalar();
    const Val& read_num_val();
    const Val& read_bin_val();
    const Val& read_coord_val();
    const Val& read_ref_val();
    const Val& read_str_val();
    const Val& read_uri_val();
    const Val& read_word_val();
    std::string read_id();
    std::string read_str_literal();
    int32_t read_two_digits(std::string errMsg);
    int32_t read_esc_char();
    
    static const char *charType;
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
};
};
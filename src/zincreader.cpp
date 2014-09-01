//
// Copyright (c) 2014, Radu Racariu, Brian Frank
// History:
//   29 Aug 2014  Radu Racariu Ported to C++
//   06 Jun 2011  Brian Frank  Creation
//
#include "zincreader.hpp"
#include "bin.hpp"
#include "bool.hpp"
#include "coord.hpp"
#include "date.hpp"
#include "datetime.hpp"
#include "marker.hpp"
#include "num.hpp"
#include "ref.hpp"
#include "str.hpp"
#include "uri.hpp"
#include "time.hpp"
#include "timezone.hpp"
#include "dict.hpp"
#include "grid.hpp"

// std
#include <sstream>
#include <stdexcept>

// boost
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>

////////////////////////////////////////////////
// ZincReader
////////////////////////////////////////////////
using namespace haystack;

ZincReader::ZincReader(std::istream& is) : m_is(is),
                                            m_cur(0),
                                            m_peek(0),
                                            m_line_num(1),
                                            m_version(0),
                                            m_is_filter(0)
{
    consume();
    consume();
}


// Read a grid
const Grid& ZincReader::read_grid()
{
    Grid* g = new Grid();
    // meta line
    read_ver();
    read_meta(g->meta());
    consume_new_line();

    // read cols
    size_t numCols = 0;
    for (;;)
    {
        std::string name = read_id();
        skip_space();
        numCols++;
        read_meta(g->addCol(name));
        if (m_cur != ',') break;
        consume();
        skip_space();
    }
    consume_new_line();

    // rows
    while (m_cur != '\n' && m_cur > 0)
    {
        Val** cells = new Val*[numCols];
        
        for (size_t i = 0; i < numCols; ++i)
        {
            skip_space();
            if (m_cur != ',' && m_cur != '\n')
                cells[i] = (Val*) &read_val();
            else
                cells[i] = new EmptyVal();

            skip_space();
            if (i + 1 < numCols)
            {
                if (m_cur != ',') throw std::runtime_error("Expecting comma in row");
                consume();
            }
        }
        consume_new_line();
        g->addRow(cells, numCols);
    }
    if (m_cur == '\n') consume_new_line();

    return *g;
}

std::string ZincReader::read_id()
{
    if (!is_id_start(m_cur)) throw std::runtime_error("Invalid name start char");
    
    std::stringstream s;
    
    while (is_id(m_cur)) { s << (char)m_cur; consume(); }
    return s.str();
}

void ZincReader::read_meta(Dict& d)
{
    // parse pairs
    while (is_id_start(m_cur))
    {
        // name
        std::string name = read_id();

        // marker or :val
        Val* val = new Marker();
        skip_space();
        if (m_cur == ':')
        {
            consume();
            skip_space();
            val = (Val*)&read_val();
            skip_space();
        }
        d.add(name, val);
        skip_space();
    }
}

void ZincReader::read_ver()
{
    std::string id = read_id();
    if (id !=("ver")) throw std::runtime_error("Expecting zinc header 'ver:2.0', not '");
    if (m_cur != ':') throw std::runtime_error("Expecting ':' colon");
    consume();
    std::string ver = read_str_literal();
    if (ver == "2.0") m_version = 2;
    else throw std::runtime_error("Unsupported zinc version: " + ver);
    skip_space();
}

const Val& ZincReader::read_scalar()
{
    const Val& val = read_val();
    if (m_cur >= 0) throw std::runtime_error("Expected end of stream");
    return val;
}

const Val& ZincReader::read_val()
{
    if (is_digit(m_cur)) return read_num_val();
    if (is_alpha(m_cur)) return read_word_val();

    switch (m_cur)
    {
    case '@': return read_ref_val();
    case '"': return read_str_val();
    case '`': return read_uri_val();
    case '-':
        if (m_peek == 'I') return read_word_val();
        return read_num_val();
    default:  throw std::runtime_error("Unexpected char for start of value");
    }
}

const Val& ZincReader::read_word_val()
{
    // read into string
    std::stringstream s;
    do { s << (char) m_cur; consume(); } while (is_alpha(m_cur));
    std::string word = s.str();

    // match identifier
    if (m_is_filter)
    {
        if (word == "true")  return *new Bool(true);
        if (word == "false") return *new Bool(false);
    }
    else
    {
        if (word == "N")   return *new EmptyVal();
        if (word == "M")   return *new Marker();
        if (word == "R")   return *new Str("_remove_");
        if (word == "T")   return *new Bool(true);
        if (word == "F")   return *new Bool(true);
        if (word == "Bin") return read_bin_val();
        if (word == "C")   return read_coord_val();
    }
    if (word == "NaN") return *new Num(Num::NaN.value);
    if (word == "INF") return *new Num(Num::POS_INF.value);
    if (word == "-INF") return *new Num(Num::NEG_INF.value);
    throw std::runtime_error("Unknown value identifier: " + word);
}

const Val& ZincReader::read_bin_val()
{
    if (m_cur < 0) throw std::runtime_error("Expected '(' after Bin");
    consume();
    std::stringstream s;
    while (m_cur != ')')
    {
        if (m_cur < 0) throw std::runtime_error("Unexpected end of bin literal");
        if (m_cur == '\n' || m_cur == '\r') throw std::runtime_error("Unexpected newline in bin literal");
        s << (char) m_cur;
        consume();
    }
    consume();
    return *new Bin(s.str());
}

const Val& ZincReader::read_coord_val()
{
    if (m_cur < 0) throw std::runtime_error("Expected '(' after Coord");
    consume();
    std::stringstream s;
    s << "C(";

    while (m_cur != ')')
    {
        if (m_cur < 0) throw std::runtime_error("Unexpected end of coord literal");
        if (m_cur == '\n' || m_cur == '\r') throw std::runtime_error("Unexpected newline in coord literal");
        s << (char) m_cur;
        consume();
    }
    consume();
    s << ")";
    const Coord& c = Coord::make(s.str());
    return *new Coord(c.lat(), c.lng());
}

const Val& ZincReader::read_num_val()
{
    // parse numeric part
    std::stringstream s;
    s << (char)m_cur;
    consume();
    while (is_digit(m_cur) || m_cur == '.' || m_cur == '_')
    {
        if (m_cur != '_') s << (char) m_cur;
        consume();
        if (m_cur == 'e' || m_cur == 'E')
        {
            if (m_peek == '-' || m_peek == '+' || is_digit(m_peek))
            {
                s << (char) m_cur; consume();
                s << (char) m_cur; consume();
            }
        }
    }
    double val = boost::lexical_cast<double>(s.str());

    // Date - check for dash
    Date* date = NULL;
    Time* time = NULL;

    int hour = -1;
    if (m_cur == '-')
    {
        int32_t year;
        try { year = boost::lexical_cast<int>(s.str()); }
        catch (std::exception&) { throw std::runtime_error("Invalid year for date value: "); }
        consume(); // dash
        int month = read_two_digits("Invalid digit for month in date value");
        if (m_cur != '-') throw std::runtime_error("Expected '-' for date value");
        consume();
        int day = read_two_digits("Invalid digit for day in date value");
        date = new Date(year, month, day);

        // check for 'T' date time
        if (m_cur != 'T') return *date;

        // parse next two digits and drop down to HTime parsing
        consume();
        hour = read_two_digits("Invalid digit for hour in date time value");
    }

    // HTime - check for colon
    if (m_cur == ':')
    {
        // hour (may have been parsed already in date time)
        if (hour < 0)
        {
            if (s.str().size() != 2) { throw std::runtime_error("Hour must be two digits for time value: "); }
            try { hour = boost::lexical_cast<int>(s.str()); }
            catch (std::exception&) { throw std::runtime_error("Invalid hour for time value: "); }
        }
        consume(); // colon
        int min = read_two_digits("Invalid digit for minute in time value");
        if (m_cur != ':') throw std::runtime_error("Expected ':' for time value");
        consume();
        int sec = read_two_digits("Invalid digit for seconds in time value");
        int ms = 0;
        if (m_cur == '.')
        {
            consume();
            int places = 0;
            while (is_digit(m_cur))
            {
                ms = (ms * 10) + (m_cur - '0');
                consume();
                places++;
            }
            switch (places)
            {
            case 1: ms *= 100; break;
            case 2: ms *= 10; break;
            case 3: break;
            default: throw std::runtime_error("Too many digits for milliseconds in time value");
            }
        }
        time = new Time(hour, min, sec, ms);
        if (date == NULL) return *time;
    }

    // HDateTime (if we have date and time)
    bool zUtc = false;
    if (date != NULL)
    {
        // timezone offset "Z" or "-/+hh:mm"
        int tzOffset = 0;
        if (m_cur == 'Z') { consume(); zUtc = true; }
        else
        {
            bool neg = m_cur == '-';
            if (m_cur != '-' && m_cur != '+') throw std::runtime_error("Expected -/+ for timezone offset");
            consume();
            int tzHours = read_two_digits("Invalid digit for timezone offset");
            if (m_cur != ':') throw std::runtime_error("Expected colon for timezone offset");
            consume();
            int tzMins = read_two_digits("Invalid digit for timezone offset");
            tzOffset = (tzHours * 3600) + (tzMins * 60);
            if (neg) tzOffset = -tzOffset;
        }

        // timezone name
        boost::scoped_ptr<TimeZone> tz;
        if (m_cur != ' ')
        {
            if (!zUtc)
                throw std::runtime_error("Expected space between timezone offset and name");
            else
                tz.reset(new TimeZone(TimeZone::UTC.name, TimeZone::UTC.offset));
        }
        else if (zUtc && !('A' <= m_peek && m_peek <= 'Z'))
        {
            tz.reset(new TimeZone(TimeZone::UTC.name, TimeZone::UTC.offset));
        }
        else
        {
            consume();
            std::stringstream tzBuf;
            if (!is_tz(m_cur)) throw std::runtime_error("Expected timezone name");
            while (is_tz(m_cur)) { tzBuf << (char) m_cur; consume(); }
            tz.reset(new TimeZone(tzBuf.str()));
        }
        return * new DateTime(*date, *time, *tz, tzOffset);
    }

    // if we have unit, parse that
    std::string unit;
    if (is_unit(m_cur))
    {
        std::stringstream s;
        while (is_unit(m_cur)) { s << (char) m_cur; consume(); }
        unit = s.str();
    }

    return * new Num(val, unit);
}

int32_t ZincReader::read_two_digits(std::string errMsg)
{
    if (!is_digit(m_cur)) throw std::runtime_error(errMsg);
    int tens = (m_cur - '0') * 10;
    consume();
    if (!is_digit(m_cur)) throw std::runtime_error(errMsg);
    int val = tens + (m_cur - '0');
    consume();
    return val;
}

const Val& ZincReader::read_ref_val()
{
    consume(); // opening @
    std::stringstream s;

    while (Ref::is_id_char(m_cur))
    {
        if (m_cur < 0) throw std::runtime_error("Unexpected end of ref literal");
        if (m_cur == '\n' || m_cur == '\r') throw std::runtime_error("Unexpected newline in ref literal");
        s << (char) m_cur;
        consume();
    }
    skip_space();

    std::string dis;
    if (m_cur == '"') dis = read_str_literal();

    return * new Ref(s.str(), dis);
}

const Val& ZincReader::read_str_val()
{
    return *new Str(read_str_literal());
}

std::string ZincReader::read_str_literal()
{
    consume(); // opening quote
    std::stringstream s;
    while (m_cur != '"')
    {
        if (m_cur < 0) throw std::runtime_error("Unexpected end of str literal");
        if (m_cur == '\n' || m_cur == '\r') throw std::runtime_error("Unexpected newline in str literal");
        if (m_cur == '\\')
        {
            s << (char) read_esc_char();
        }
        else
        {
            s << (char) m_cur;
            consume();
        }
    }
    consume(); // closing quote
    return s.str();
}

int32_t ZincReader::read_esc_char()
{
    consume();  // back slash

    // check basics
    switch (m_cur)
    {
    case 'b':   consume(); return '\b';
    case 'f':   consume(); return '\f';
    case 'n':   consume(); return '\n';
    case 'r':   consume(); return '\r';
    case 't':   consume(); return '\t';
    case '"':   consume(); return '"';
    case '$':   consume(); return '$';
    case '\\':  consume(); return '\\';
    }

    // check for uxxxx
    if (m_cur == 'u')
    {
        consume();
        int n3 = to_nibble(m_cur); consume();
        int n2 = to_nibble(m_cur); consume();
        int n1 = to_nibble(m_cur); consume();
        int n0 = to_nibble(m_cur); consume();
        return (n3 << 12) | (n2 << 8) | (n1 << 4) | (n0);
    }

    throw std::runtime_error("Invalid escape sequence: \\");
}

int32_t ZincReader::to_nibble(int32_t c)
{
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    throw std::runtime_error("Invalid hex char");
}

const Val& ZincReader::read_uri_val()
{
    consume(); // opening backtick
    std::stringstream s;

    for (;;)
    {
        if (m_cur < 0) throw std::runtime_error("Unexpected end of uri literal");
        if (m_cur == '\n' || m_cur == '\r') throw std::runtime_error("Unexpected newline in uri literal");
        if (m_cur == '`') break;
        if (m_cur == '\\')
        {
            switch (m_peek)
            {
            case ':': case '/': case '?': case '#':
            case '[': case ']': case '@': case '\\':
            case '&': case '=': case ';':
                s << (char) m_cur;
                s << (char) m_peek;
                consume();
                consume();
                break;
            case '`':
                s << '`';
                consume();
                consume();
                break;
            default:
                if (m_peek == 'u' || m_peek == '\\') s << (char)read_esc_char();
                else throw std::runtime_error("Invalid URI escape sequence \\");
                break;
            }
        }
        else
        {
            s << (char) m_cur;
            consume();
        }
    }
    consume(); // closing backtick
    return * new Uri(s.str());
}

void ZincReader::skip_space()
{
    while (m_cur == ' ' || m_cur == '\t') consume();
}

void ZincReader::consume_new_line()
{
    if (m_cur != '\n') throw std::runtime_error("Expecting newline");
    consume();
}

void ZincReader::consume()
{
    m_cur = m_peek;
    int c = m_is.get();
   
    //if (!m_is.good())
        //throw std::runtime_error("Bad stream.");

    m_peek = c;
    if (m_cur == '\n') m_line_num++;

}

#define DIGIT 0x01
#define ALPHA_LO 0x02
#define ALPHA_UP 0x04
#define ALPHA  (ALPHA_UP | ALPHA_LO)
#define UNIT 0x08
#define TZ  0x10
#define ID_START 0x20
#define ID 0x40

char* ZincReader::init_table()
{
    static char charTypes[128];
    for (int i = '0'; i <= '9'; ++i) charTypes[i] = (DIGIT | TZ | ID);
    for (int i = 'a'; i <= 'z'; ++i) charTypes[i] = (ALPHA_LO | UNIT | TZ | ID_START | ID);
    for (int i = 'A'; i <= 'Z'; ++i) charTypes[i] = (ALPHA_UP | UNIT | TZ | ID);
    charTypes['%'] = UNIT;
    charTypes['_'] = UNIT | TZ | ID;
    charTypes['/'] = UNIT;
    charTypes['$'] = UNIT;
    charTypes['-'] = TZ;
    charTypes['+'] = TZ;
    return (char*)charTypes;
}

const char *ZincReader::charType = ZincReader::init_table();

bool ZincReader::is_digit(int c)   { return c > 0 && c < 128 && (charType[c] & DIGIT) != 0; }
bool ZincReader::is_alpha(int c)   { return c > 0 && c < 128 && (charType[c] & ALPHA) != 0; }
bool ZincReader::is_unit(int c)    { return c > 0 && (c >= 128 || (charType[c] & UNIT) != 0); }
bool ZincReader::is_tz(int c)      { return c > 0 && c < 128 && (charType[c] & TZ) != 0; }
bool ZincReader::is_id_start(int c) { return c > 0 && c < 128 && (charType[c] & ID_START) != 0; }
bool ZincReader::is_id(int c)      { return c > 0 && c < 128 && (charType[c] & ID) != 0; }
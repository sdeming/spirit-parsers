#ifndef __uri_percent_encoded_char_h__
#define __uri_percent_encoded_char_h__

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    // Symbol table for individual 4 bit hex characters.
    struct hex_digit_ : qi::symbols<char, char>
    {
        hex_digit_()
        {
            add
                ("0", 0x0) ("1", 0x1) ("2", 0x2) ("3", 0x3) ("4", 0x4)
                ("5", 0x5) ("6", 0x6) ("7", 0x7) ("8", 0x8) ("9", 0x9)
                ("A", 0xa) ("a", 0xa)
                ("B", 0xb) ("b", 0xb)
                ("C", 0xc) ("c", 0xc)
                ("D", 0xd) ("d", 0xd)
                ("E", 0xe) ("e", 0xe)
                ("F", 0xf) ("f", 0xf)
            ;
        }
    } hex_digit;

    /**
     * Implementation of the percent encoded character grammar.
     * 
     * Reads a percent encoded string representing a single ASCII character
     * and produces that single ascii character.
     *
     * Examples: %6d = m
     *           %20 = <space>
     *           %2e = .
     */
    template <typename Iterator>
    struct percent_encoded_char : qi::grammar<Iterator, char()>
    {
        percent_encoded_char() :
            percent_encoded_char::base_type(start)
        {
            start = qi::eps[qi::_val = 0] >> 
                    (
                      qi::omit['%'] >> 
                      hex_digit[qi::_val =  qi::_1 * 16] >> 
                      hex_digit[qi::_val += qi::_1]
                    );
        }

        qi::rule<Iterator, char()> start;
    };
} // namespace uri

#endif // __uri_percent_encoded_char_h__


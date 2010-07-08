#ifndef __ipv4_address_h__
#define __ipv4_address_h__

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <iostream>
#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    /**
     * Implementation of an ipv4 IP address parser
     */
    template <typename Iterator>
    struct ipv4_address : qi::grammar<Iterator, std::string()>
    {
        ipv4_address() : 
            ipv4_address::base_type(start)
        {
            using qi::char_;
            using qi::raw;
            using ascii::digit;
            using ascii::string;

            qi::on_error<qi::fail>
            (
                start,
                std::cerr << phoenix::val("Error! Expecting")
                          << qi::_4
                          << phoenix::val(" here: \"")
                          << phoenix::construct<std::string>(qi::_3, qi::_2)
                          << phoenix::val("\"")
                          << std::endl
            );

            dec_octet = string("25") >> char_("012345")
                      | char_('2')  >> char_("01234") >> digit
                      | char_("01") >> digit >> digit
                      | digit >> digit
                      | digit
                      ;

            start     = raw[(dec_octet >> qi::repeat(3)[char_('.') >> dec_octet])] >> !digit
                      ;

            dec_octet.name("dec_octet");
            start.name("start");
        }

        qi::rule<Iterator> dec_octet;
        qi::rule<Iterator, std::string()> start;
    }; // struct ipv4_address
} // namespace uri

#endif // __ipv4_address_h__


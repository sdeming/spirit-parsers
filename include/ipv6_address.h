#ifndef __ipv6_parser_h__
#define __ipv6_parser_h__

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "ipv4_address.h"

#include <iostream>
#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    /**
     * Implementation of an ipv6 IP address parser
     */
    template <typename Iterator>
    struct ipv6_address : qi::grammar<Iterator, std::string()>
    {
        ipv6_address() : 
            ipv6_address::base_type(start)
        {
            using qi::repeat;
            using qi::raw;
            using ascii::xdigit;

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


            h16          = repeat(1,4)[xdigit];    
            ls32         = h16 >> ':' >> h16 | ipv4 ;

            ipv6_attr    =                                                repeat(6)[h16 >> ':'] >> ls32
                         |                                        "::" >> repeat(5)[h16 >> ':'] >> ls32
                         | - h16                               >> "::" >> repeat(4)[h16 >> ':'] >> ls32
                         | -(h16 >> -(           ':' >> h16) ) >> "::" >> repeat(3)[h16 >> ':'] >> ls32
                         | -(h16 >> -repeat(1,2)[':' >> h16] ) >> "::" >> repeat(2)[h16 >> ':'] >> ls32
                         | -(h16 >> -repeat(1,3)[':' >> h16] ) >> "::" >>           h16 >> ':'  >> ls32
                         | -(h16 >> -repeat(1,4)[':' >> h16] ) >> "::"                          >> ls32
                         | -(h16 >> -repeat(1,5)[':' >> h16] ) >> "::" >>           h16
                         | -(h16 >> -repeat(1,6)[':' >> h16] ) >> "::"
                         ;

            start        = raw[ipv6_attr];

            h16.name("h16");
            ls32.name("ls32");
            ipv6_attr.name("ipv6_attr");
            start.name("start");
        }

        ipv4_address<Iterator> ipv4;
        qi::rule<Iterator> h16, ls32;
        qi::rule<Iterator, std::string()> ipv6_attr;
        qi::rule<Iterator, std::string()> start;
    };
} // namespace uri

#endif // __ipv6_parser_h__


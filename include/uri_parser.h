#ifndef __uri_parser_h__
#define __uri_parser_h__

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <iostream>
#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    using qi::char_;
    using qi::rule;
    using qi::omit;
    using qi::xdigit;
    using qi::repeat;
    using qi::lexeme;
    using qi::raw;
    using qi::_val;
    using qi::_1;

    using ascii::alnum;
    using ascii::digit;
    using ascii::string;

    using phoenix::ref;

    struct uri_type
    {
        std::string scheme;
        std::string user_info;
        std::string host;
        std::string port;
        std::string path;
        std::string query;
        std::string fragment;

        std::string to_string()
        {
            return "scheme(" + scheme + 
                   "),user_info(" + user_info + 
                   "),host(" + host + 
                   "),port(" + port + 
                   "),path(" + path + 
                   "),query(" + query + 
                   "),fragment(" + fragment + 
                   ")";
        }
    };

    template <typename Iterator>
    struct ipv4_address : qi::grammar<Iterator, std::string()>
    {
        ipv4_address() : 
            ipv4_address::base_type(ipv4)
        {
            using namespace qi::labels;
            using qi::on_error;
            using qi::fail;
            using qi::debug;
            using phoenix::construct;
            using phoenix::val;

            on_error<fail>
            (
                ipv4,
                std::cerr << val("Error! Expecting")
                          << _4
                          << val(" here: \"")
                          << construct<std::string>(_3, _2)
                          << val("\"")
                          << std::endl
            );

            dec_octet = string("25") >> char_("012345")
                      | char_('2')  >> char_("01234") >> digit
                      | char_("01") >> digit >> digit
                      | digit >> digit
                      | digit
                      ;

            ipv4      = raw[(dec_octet >> repeat(3)[char_('.') >> dec_octet])] >> !digit
                      ;

            dec_octet.name("dec_octet");
            ipv4.name("ipv4");

            //debug(dec_octet);
            //debug(ipv4);
        }

        rule<Iterator> dec_octet;
        rule<Iterator, std::string()> ipv4;
    };

    template <typename Iterator>
    struct ipv6_address : qi::grammar<Iterator, std::string()>
    {
        ipv6_address() : 
            ipv6_address::base_type(ipv6)
        {
            using namespace qi::labels;
            using qi::on_error;
            using qi::fail;
            using qi::debug;
            using phoenix::construct;
            using phoenix::val;

            on_error<fail>
            (
                ipv6,
                std::cerr << val("Error! Expecting")
                          << _4
                          << val(" here: \"")
                          << construct<std::string>(_3, _2)
                          << val("\"")
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

            ipv6         = raw[ipv6_attr];

            h16.name("h16");
            ls32.name("ls32");
            ipv6_attr.name("ipv6_attr");
            ipv6.name("ipv6");

            //debug(h16);
            //debug(ls32);
            //debug(ipv6_attr);
            //debug(ipv6);
        }

        ipv4_address<Iterator> ipv4;
        rule<Iterator> h16, ls32;
        rule<Iterator, std::string()> ipv6_attr;
        rule<Iterator, std::string()> ipv6;
    };

    template <typename Iterator>
    struct uri_parser : qi::grammar<Iterator>
    {
        uri_parser(uri_type &it) : 
            uri_parser::base_type(uri)
        {
            escaped_char    %= char_('%') >> xdigit >> xdigit; /// yield the converter char?
            mark_char       %= char_("-_.~");
            gen_delims      %= char_(":/?#[]@");
            sub_delims      %= char_("!$&'()*+,;=");
            reserved_char   %= gen_delims | sub_delims;
            unreserved_char %= alnum | mark_char;

            pchar           = unreserved_char 
                            | sub_delims 
                            | char_(":@")
                            | escaped_char  
                            ;

            // Scheme
            scheme_attr     = alnum >> +(alnum | char_("+-.")) >> omit[':'];
            scheme          = scheme_attr[ref(it.scheme) = qi::_1];

            // User info
            user_info_attr  = +(unreserved_char | sub_delims | escaped_char | ':') >> omit['@'];
            user_info       = user_info_attr[ref(it.user_info) = qi::_1];

            // Address
            ip_v_future     = char_('v') >> repeat(0,1)[xdigit] >> '.' >> repeat(0,1)[unreserved_char | sub_delims | ':'];
            ip_literal      = char_('[') >> (ip_v_future | ipv6) >> ']';
            host_attr       = ipv4
                            | ip_literal
                            | raw[+(unreserved_char | escaped_char | sub_delims)]
                            ;
            host            = host_attr[ref(it.host) = qi::_1];

            port_attr       = *digit;
            port            = port_attr[ref(it.port) = qi::_1];

            // Authority
            authority       = -user_info >> host >> -(':' >> port);

            // Path
            path_attr       = char_('/') >> *(alnum | char_("+-./"));
            path            = path_attr[ref(it.path) = qi::_1];

            // Query
            query_attr      = omit['?'] >> *(pchar | char_("/?:@"));
            query           = query_attr[ref(it.query) = qi::_1];

            // Query String
            fragment_attr   = omit['#'] >> *(pchar | char_("/?"));
            fragment        = fragment_attr[ref(it.fragment) = qi::_1];

            // Request-URI
            abs_uri         = scheme >> "//" >> authority >> -path >> -query >> -fragment;
            abs_path        = path;
            all_star        = string("*");

            // entry
            uri             = abs_uri | abs_path | all_star;
        }

        ipv4_address<Iterator> ipv4;
        ipv6_address<Iterator> ipv6;

        rule<Iterator> escaped_char;
        rule<Iterator, char()> gen_delims, sub_delims;
        rule<Iterator, char()> mark_char, reserved_char, unreserved_char;
        rule<Iterator, char()> pchar;

        rule<Iterator, std::string()> ip_v_future, ip_literal;

        rule<Iterator> scheme, user_info, host, port, path, query, fragment, authority;
        rule<Iterator, std::string()> scheme_attr, user_info_attr, host_attr, port_attr, path_attr, query_attr, fragment_attr;

        rule<Iterator, std::string()> all_star;
        rule<Iterator, std::string()> abs_uri, abs_path, request_uri;

        rule<Iterator> uri;
    };
} // namespace uri

#endif // __uri_parser_h__


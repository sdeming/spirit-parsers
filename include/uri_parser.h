#ifndef __uri_parser_h__
#define __uri_parser_h__

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "percent_encoded_char.h"
#include "ipv4_address.h"
#include "ipv6_address.h"

#include <iostream>
#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    struct uri_t
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
    struct uri_parser : qi::grammar<Iterator>
    {

        uri_parser(uri_t &it) : 
            uri_parser::base_type(start)
        {
            using qi::omit;
            using qi::xdigit;
            using qi::repeat;
            using qi::char_;
            using qi::raw;
            using ascii::alnum;
            using ascii::digit;
            using ascii::string;
            using phoenix::ref;

            gen_delims      = char_(":/?#[]@");
            sub_delims      = char_("!$&'()*+,;=");
            reserved_char   = gen_delims | sub_delims;
            unreserved_char = alnum | char_("-._~");
            pchar           = unreserved_char | pct_enc_char | sub_delims | char_(":@");

            // Segments
            segment         = *pchar;
            segment_nz      = +pchar;
            segment_nz_nc   = +(unreserved_char | pct_enc_char | sub_delims | char_('@'));

            // Scheme
            scheme_attr     = alnum >> +(alnum | char_("+-.")) >> omit[':'];
            scheme          = scheme_attr[ref(it.scheme) = qi::_1];

            // User info
            user_info_attr  = +(unreserved_char | pct_enc_char | sub_delims | ':') >> omit['@'];
            user_info       = user_info_attr[ref(it.user_info) = qi::_1];

            // Address
            reg_name        = +(unreserved_char | pct_enc_char | sub_delims);
            ip_v_future     = char_('v') >> -xdigit >> '.' >> repeat(0,1)[unreserved_char | sub_delims | ':'];
            ip_literal      = omit['['] >> (ip_v_future | ipv6) >> omit[']'];
            host_attr       = ip_literal | ipv4 | reg_name;
            host            = host_attr[ref(it.host) = qi::_1];
            port_attr       = *digit;
            port            = port_attr[ref(it.port) = qi::_1];

            // Authority
            authority       = -user_info >> host >> -(':' >> port);

            // Path
            path_abempty    = raw[*("/" >> segment)];
            path_absolute   = raw[string("/") >> -(segment_nz >> *("/" >> segment))];
            path_noscheme   = raw[segment_nz_nc >> *("/" >> segment)];
            path_rootless   = raw[segment_nz >> *("/" >> segment)];
            path_empty      = !pchar;

            // Query
            query_attr      = omit['?'] >> *(pchar | char_("/?"));
            query           = query_attr[ref(it.query) = qi::_1];

            // Fragment
            fragment_attr   = omit['#'] >> *(pchar | char_("/?"));
            fragment        = fragment_attr[ref(it.fragment) = qi::_1];

            // Request-URI
            hier_part       = string("//") >> authority >> -path_abempty[ref(it.path) = qi::_1]
                            | path_absolute[ref(it.path) = qi::_1]
                            | path_rootless[ref(it.path) = qi::_1]
                            | path_empty[ref(it.path) = qi::_1]
                            ;
            abs_uri         = scheme >> hier_part >> -query >> -fragment;

            relative_part   = string("//") >> authority >> -path_absolute[ref(it.path) = qi::_1]
                            | path_absolute[ref(it.path) = qi::_1]
                            | path_rootless[ref(it.path) = qi::_1]
                            | path_empty[ref(it.path) = qi::_1]
                            ;
            rel_uri         = relative_part >> -query >> -fragment;

            // entry
            start           = abs_uri | rel_uri | string("*");
        }

        ipv4_address<Iterator> ipv4;
        ipv6_address<Iterator> ipv6;
        percent_encoded_char<Iterator> pct_enc_char;

        qi::rule<Iterator, std::string()> reg_name;

        qi::rule<Iterator, char()> gen_delims, sub_delims;
        qi::rule<Iterator, char()> reserved_char, unreserved_char;
        qi::rule<Iterator, char()> pchar;

        qi::rule<Iterator> segment, segment_nz, segment_nz_nc;
        qi::rule<Iterator, std::string()> path_abempty, path_absolute, path_noscheme, path_rootless, path_empty;

        qi::rule<Iterator, std::string()> ip_v_future, ip_literal;

        qi::rule<Iterator> scheme, user_info, host, port, path, query, fragment, authority;

        qi::rule<Iterator, std::string()> scheme_attr, user_info_attr, host_attr, port_attr, path_attr, query_attr, fragment_attr;
        qi::rule<Iterator, std::string()> hier_part, relative_part, abs_uri, rel_uri;

        qi::rule<Iterator> start;
    };
} // namespace uri

#endif // __uri_parser_h__


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
            using ascii::alnum;
            using ascii::digit;
            using ascii::string;
            using phoenix::ref;

            mark_char       = char_("-_.~");
            gen_delims      = char_(":/?#[]@");
            sub_delims      = char_("!$&'()*+,;=");
            reserved_char   = gen_delims | sub_delims;
            unreserved_char = alnum | mark_char;

            pchar           = unreserved_char | sub_delims | char_(":@") | pct_enc_char;

            // Scheme
            scheme_attr     = alnum >> +(alnum | char_("+-.")) >> omit[':'];
            scheme          = scheme_attr[ref(it.scheme) = qi::_1];

            // User info
            user_info_attr  = +(unreserved_char | sub_delims | pct_enc_char | ':') >> omit['@'];
            user_info       = user_info_attr[ref(it.user_info) = qi::_1];

            // Address
            nic_name        = +(unreserved_char | sub_delims | pct_enc_char);
            ip_v_future     = char_('v') >> -xdigit >> '.' >> repeat(0,1)[unreserved_char | sub_delims | ':'];
            ip_literal      = omit['['] >> (ip_v_future | ipv6) >> omit[']'];
            host_attr       = ipv4 | ip_literal | nic_name;
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

            // entry
            start           = abs_uri | abs_path | string("*");
        }

        ipv4_address<Iterator> ipv4;
        ipv6_address<Iterator> ipv6;
        percent_encoded_char<Iterator> pct_enc_char;

        qi::rule<Iterator, std::string()> nic_name;

        qi::rule<Iterator, char()> gen_delims, sub_delims;
        qi::rule<Iterator, char()> mark_char, reserved_char, unreserved_char;
        qi::rule<Iterator, char()> pchar;

        qi::rule<Iterator, std::string()> ip_v_future, ip_literal;

        qi::rule<Iterator> scheme, user_info, host, port, path, query, fragment, authority;
        qi::rule<Iterator, std::string()> scheme_attr, user_info_attr, host_attr, port_attr, path_attr, query_attr, fragment_attr;

        qi::rule<Iterator, std::string()> abs_uri, abs_path, request_uri;

        qi::rule<Iterator> start;
    };
} // namespace uri

#endif // __uri_parser_h__


#ifndef __uri_parser_h__
#define __uri_parser_h__

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <string>

namespace uri
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    using qi::char_;
    using qi::rule;
    using qi::lit;
    using qi::lexeme;
    using qi::skip;
    using qi::omit;
    using qi::xdigit;
    using qi::repeat;
    using qi::raw;
    using namespace qi::labels;

    using ascii::space;
    using ascii::alpha;
    using ascii::alnum;
    using ascii::upper;
    using ascii::print;
    using ascii::digit;
    using ascii::string;

    using phoenix::ref;
    using phoenix::cref;
    using phoenix::val;
    using phoenix::construct;
    using phoenix::insert;

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
            host_attr       = +(unreserved_char | escaped_char | sub_delims);
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
            uri             = abs_uri | abs_path | all_star;
        }

        rule<Iterator> uri;
        rule<Iterator> escaped_char;
        rule<Iterator, char()> gen_delims, sub_delims;
        rule<Iterator, char()> mark_char, reserved_char, unreserved_char;
        rule<Iterator, char()> pchar;

        rule<Iterator> method, scheme, user_info, host, port, path, query, fragment, authority;
        rule<Iterator, std::string()> method_attr, scheme_attr, user_info_attr, host_attr, port_attr, path_attr, query_attr, fragment_attr;
        rule<Iterator, std::string()> all_star;
        rule<Iterator, std::string()> abs_uri, abs_path, request_uri;
    };
} // namespace uri

#endif // __uri_parser_h__


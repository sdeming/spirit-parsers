#ifndef __http11_parser_h__
#define __http11_parser_h__

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <string>
#include <vector>
#include <map>

namespace http11
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

    typedef std::string header_key_type;
    typedef std::string header_value_type;
    typedef std::map<header_key_type, header_value_type> header_container_type;
    typedef header_container_type::value_type header_container_value_type;

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

    struct request_type
    {
        std::string method;
        std::string version;
        uri_type uri;
        header_container_type headers;

        request_type()
        {
        }

        void dump()
        {
            std::cerr << "request line:"
                      << "method("  << method  << ")" 
                      << "version(" << version << ")"
                      << "uri("     << uri.to_string() << ")"
                      << std::endl;

            std::cerr << "headers:" << std::endl;
            header_container_type::const_iterator cur = headers.begin();
            header_container_type::const_iterator end = headers.end();
            for (; cur != end; ++cur) {
                std::cerr << "key("   << (*cur).first  << ")"
                          << "value(" << (*cur).second << ")"
                          << std::endl;
            }
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
            uri             = ( abs_uri | abs_path | all_star) >> space;
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

    template <typename Iterator>
    struct request_parser : qi::grammar<Iterator>
    {
        request_parser(request_type &it) : 
            request_parser::base_type(request),
            uri(it.uri)
        {
            crlf            %= string("\r\n");
            escaped_char    %= char_('%') >> xdigit >> xdigit;
            reserved_char   %= char_(";/?:@&=+$,");
            mark_char       %= char_("-_.!~*\'()");

            // Method 
            method_attr     = repeat(1, 20)[upper | digit];
            method          = method_attr[ref(it.method) = qi::_1] >> space;

            // HTTP-Version 
            version_attr    = +digit >> char_('.') >> +digit;
            version         = string("HTTP/") >> version_attr[ref(it.version) = qi::_1];

            // Full Request-Line
            http_request    = method >> uri >> version
                            ;

            // Headers: key: value\r\n[key: value\r\n...]
            //TODO: continuation lines
            header_key      = +(alnum | '-');
            header_value    = +(print | ' ' | '\t');
            header          = (header_key >> ':' >> omit[*space] >> header_value)[
                insert(ref(it.headers), construct<header_container_value_type>(qi::_1, qi::_2))
            ];

            //TODO: where does the input stream begin? How do we pass that back to the parser?
            request = http_request >> crlf >> *(header >> crlf);
        }
       
        rule<Iterator> request;
        uri_parser<Iterator> uri;

        rule<Iterator> crlf;
        rule<Iterator> http_request;

        rule<Iterator> mark_char, escaped_char, reserved_char;

        rule<Iterator> method, version;
        rule<Iterator, std::string()> method_attr, version_attr;

        rule<Iterator, std::string()> header_key, header_value;

        rule<Iterator, header_container_value_type> header;
    };

} // namespace http11

#endif // __http11_parser_h__


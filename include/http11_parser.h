#ifndef __http11_parser_h__
#define __http11_parser_h__

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <string>
#include <map>

#include "uri_parser.h"

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

    struct request_type
    {
        std::string method;
        std::string version;
        uri::uri_type uri;
        header_container_type headers;

        request_type()
        {
        }

        void dump()
        {
            std::cerr << "request line:"
                      << "method("  << method  << ")," 
                      << "version(" << version << "),"
                      << "uri("     << uri.to_string() << "),"
                      << std::endl;

            std::cerr << "headers:" << std::endl;
            header_container_type::const_iterator cur = headers.begin();
            header_container_type::const_iterator end = headers.end();
            for (; cur != end; ++cur) {
                std::cerr << "key("   << (*cur).first  << "),"
                          << "value(" << (*cur).second << ")"
                          << std::endl;
            }
        }
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
            http_request    = method >> uri >> ' ' >> version;

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
        uri::uri_parser<Iterator> uri;

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


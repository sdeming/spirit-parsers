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

    typedef std::string header_key_t;
    typedef std::string header_value_t;
    typedef std::map<header_key_t, header_value_t> header_container_t;
    typedef header_container_t::value_type header_container_value_type;

    /**
     * Structure representing an entire HTTP 1.1 request.
     */
    struct request_t
    {
        std::string method;
        std::string version;
        uri::uri_t uri;
        header_container_t headers;

        request_t() { }

        std::string to_string()
        {
            std::ostringstream str;
            str << "request line:"
                << "method("  << method  << ")," 
                << "version(" << version << "),"
                << "uri("     << uri.to_string() << "),"
                << std::endl;

            std::cerr << "headers:" << std::endl;
            header_container_t::const_iterator cur = headers.begin();
            header_container_t::const_iterator end = headers.end();
            for (; cur != end; ++cur) {
                str << "key("   << (*cur).first  << "),"
                    << "value(" << (*cur).second << ")"
                    << std::endl;
            }

            return str.str();
        }
    };

    /**
     * An implementation of an HTTP 1.1 parser
     */
    template <typename Iterator>
    struct request_parser : qi::grammar<Iterator>
    {
        request_parser(request_t &it) : 
            request_parser::base_type(start),
            uri(it.uri)
        {
            using qi::char_;
            using qi::omit;
            using qi::repeat;

            using ascii::space;
            using ascii::alnum;
            using ascii::upper;
            using ascii::print;
            using ascii::digit;
            using ascii::string;

            using phoenix::ref;
            using phoenix::val;
            using phoenix::construct;
            using phoenix::insert;

            // Misc.
            crlf            = string("\r\n");

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
            start = http_request >> crlf >> *(header >> crlf);
        }
       
        uri::uri_parser<Iterator> uri;
        qi::rule<Iterator, std::string()> crlf;
        qi::rule<Iterator> http_request;
        qi::rule<Iterator> method, version;
        qi::rule<Iterator, std::string()> method_attr, version_attr;
        qi::rule<Iterator, std::string()> header_key, header_value;
        qi::rule<Iterator, header_container_value_type> header;
        qi::rule<Iterator> start;
    };

} // namespace http11

#endif // __http11_parser_h__


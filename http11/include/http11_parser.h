#ifndef __http11_parser_h__
#define __http11_parser_h__

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

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
    using qi::hex;
    using namespace qi::labels;

    using ascii::space;
    using ascii::alpha;
    using ascii::alnum;
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
        std::string uri;
        int version_major;
        int version_minor;
        header_container_type headers;

        request_type()
        {
        }

        void dump()
        {
            std::cerr << "request line:"
                      << "method("  << method  << ")" 
                      << "uri("     << uri     << ")"
                      << "version(" << version_major << '.' << version_minor << ")" 
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

    /*
    template <typename Iterator>
    struct request_header : qi::grammar<Iterator, header_container_value_type>
    {
        request_header() : request_header::base_type(header)
        {
        }

        rule<Iterator, header_container_value_type> header;
    };
    */

    /* URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]] */

    template <typename Iterator>
    struct request_parser : qi::grammar<Iterator>
    {
        request_parser(request_type &it) : request_parser::base_type(request)
        {
            crlf            = string("\r\n");
            splat           = string("*");
            word            = +(char_ - space);
            escaped_char    = lit('%') >> hex >> hex;
            reserved_char   = lit(';') | '/' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',';
            mark_char       = lit('-') | '_' | '.' | '!' | '~' | '*' | '\'' | '(' | ')';
            unreserved_char = alnum | mark_char;


            /**
             * Request-Line: Method Request-URI HTTP-Version\r\n
             */

            // Method 
            get_method      = string("GET"    )[ref(it.method) = qi::_1];
            head_method     = string("HEAD"   )[ref(it.method) = qi::_1];
            put_method      = string("PUT"    )[ref(it.method) = qi::_1];
            post_method     = string("POST"   )[ref(it.method) = qi::_1];
            delete_method   = string("DELETE" )[ref(it.method) = qi::_1];
            options_method  = string("OPTIONS")[ref(it.method) = qi::_1];
            trace_method    = string("TRACE"  )[ref(it.method) = qi::_1];
            connect_method  = string("CONNECT")[ref(it.method) = qi::_1];

            // Request-URI
            abs_uri         = word;
            abs_path        = word;
            authority       = word;
            request_uri     = (abs_uri | abs_path | authority | splat)[ref(it.uri) = qi::_1];

            // HTTP-Version 
            version_major   = ((digit - '0') >> *digit)[ref(it.version_major) = qi::_1];
            version_minor   = ((digit - '0') >> *digit)[ref(it.version_minor) = qi::_1];
            version         = string("HTTP/") >> version_major >> '.' >> version_minor;

            // Full Request-Line
            get_request     = get_method     >> ' ' >> request_uri >> ' ' >> version;
            head_request    = head_method    >> ' ' >> request_uri >> ' ' >> version;
            put_request     = put_method     >> ' ' >> request_uri >> ' ' >> version;
            post_request    = post_method    >> ' ' >> request_uri >> ' ' >> version;
            delete_request  = delete_method  >> ' ' >> request_uri >> ' ' >> version;
            options_request = options_method >> ' ' >> request_uri >> ' ' >> version;
            trace_request   = trace_method   >> ' ' >> request_uri >> ' ' >> version;
            connect_request = connect_method >> ' ' >> request_uri >> ' ' >> version;

            // Headers: key: value\r\n[key: value\r\n...]
            //TODO: continuation lines
            header_key      = +(alnum | '-');
            header_value    = +(print | ' ' | '\t');
            header          = (header_key >> ':' >> omit[*space] >> header_value)[
                insert(ref(it.headers), construct<header_container_value_type>(qi::_1, qi::_2))
            ];

            // The full request
            //TODO: where does the input stream begin? How do we pass that back to the parser?
            request = get_request     >> crlf >> *(header >> crlf)
                    | head_request    >> crlf >> *(header >> crlf)
                    | put_request     >> crlf >> *(header >> crlf)
                    | post_request    >> crlf >> *(header >> crlf)
                    | delete_request  >> crlf >> *(header >> crlf)
                    | options_request >> crlf >> *(header >> crlf)
                    | trace_request   >> crlf >> *(header >> crlf)
                    | connect_request >> crlf >> *(header >> crlf)
                    ;
        }
       
        rule<Iterator> request;
        rule<Iterator, std::string()> crlf;
        rule<Iterator, std::string()> splat;
        rule<Iterator, std::string()> word;
        rule<Iterator, char> mark_char, escaped_char, reserved_char, unreserved_char;
        rule<Iterator, std::string()> scheme, host, port, path, filename;
        rule<Iterator, std::string()> get_method, head_method, put_method, post_method, delete_method, options_method, trace_method, connect_method;
        rule<Iterator, std::string()> get_request, head_request, put_request, post_request, delete_request, options_request, trace_request, connect_request;
        rule<Iterator, std::string()> abs_uri, abs_path, authority, request_uri;
        rule<Iterator, std::string()> version_major, version_minor, version;
        rule<Iterator, std::string()> header_key, header_value;
        rule<Iterator, header_container_value_type> header;
    };

} // namespace http11

#endif // __http11_parser_h__


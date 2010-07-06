#define BOOST_TEST_MODULE ParserTest
#include <boost/test/unit_test.hpp>

#include "http11_parser.h"

//BOOST_AUTO_TEST_SUITE(test_suite)

template <typename T>
bool P(http11::request_type &req, const T &test)
{
    using boost::spirit::ascii::space;
    using boost::spirit::qi::phrase_parse;

    typename T::const_iterator begin = test.begin();
    typename T::const_iterator end = test.end();

    http11::request_type new_request;
    req = new_request;
    
    std::cerr << "TEST:" << std::endl << test << "==================" << std::endl;
    http11::request_parser<typename T::const_iterator> grammar(req);
    bool pass = phrase_parse(begin, end, grammar, space);
    req.dump(); 
    std::cerr << "==================" << std::endl;
    return pass;
}

bool P(http11::request_type &req, const char *test)
{
    return P(req, std::string(test));
}

BOOST_AUTO_TEST_CASE(all_methods_without_headers)
{
    http11::request_type req;
    BOOST_CHECK(true == P(req, "GET / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "POST / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "PUT / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "DELETE / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "OPTIONS / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "CONNECT / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "TRACE / HTTP/1.1\r\n"));
    BOOST_CHECK(true == P(req, "HEAD / HTTP/1.1\r\n"));
}

BOOST_AUTO_TEST_CASE(with_one_header)
{
    http11::request_type req;
    BOOST_CHECK(true == P(req, "GET / HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK("makefile.com" == req.headers["Host"]);
    BOOST_CHECK("makefile.com" != req.headers["HOST"]); // case sensitive
    BOOST_CHECK("something else" != req.headers["Host"]); 
}

BOOST_AUTO_TEST_CASE(with_more_than_one_headers)
{
    http11::request_type req;
    BOOST_CHECK(true == P(req, "GET / HTTP/1.1\r\nHost: makefile.com\r\nX-Test: goobers\r\nX-Another: gobstoppers"));
    BOOST_CHECK("goobers" == req.headers["X-Test"]);
    BOOST_CHECK("gobstoppers" == req.headers["X-Another"]);
}

BOOST_AUTO_TEST_CASE(unknown_method)
{
    http11::request_type req;
    P(req, "SOMEthing / HTTP/1.1\r\nHost: makefile.com");
    BOOST_CHECK(false == P(req, "get / HTTP/1.1\r\n"));
    BOOST_CHECK(false == P(req, "GeT / HTTP/1.1\r\nHost: makefile.com"));
    BOOST_CHECK(false == P(req, "SOMEthing / HTTP/1.1\r\nHost: makefile.com"));
    BOOST_CHECK(false == P(req, "THISMETHODISTOOMANYCHARACTERSINLENGTH / HTTP/1.1\r\nHost: makefile.com"));
}

BOOST_AUTO_TEST_CASE(uri_correctness)
{
    http11::request_type req;

    BOOST_CHECK(true == P(req, "GET http://makefile.com HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET http://%6dakefile.com HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET http://makefile.com HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET http://makefile.com:80 HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET http://makefile.com:80/ HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET http://makefile.com:8080/ HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://makefile.com HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://makefile.com:443 HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://makefile.com:443/ HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://user@makefile.com:443/ HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://user:password@makefile.com:443/ HTTP/1.1\r\nHost: makefile.com\r\n"));
    BOOST_CHECK(true == P(req, "GET https://user:password@makefile.com:443/path/to/riches.html HTTP/1.1\r\nHost: makefile.com\r\n"));
}

//BOOST_AUTO_TEST_SUITE_END()

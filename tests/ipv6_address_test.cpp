#define BOOST_TEST_MODULE ParserTest
#include <boost/test/unit_test.hpp>

#include "uri_parser.h"

//BOOST_AUTO_TEST_SUITE(test_suite)

bool P(const std::string &test)
{
    using std::string;
    using boost::spirit::ascii::space;
    using boost::spirit::qi::phrase_parse;

    string::const_iterator begin = test.begin();
    string::const_iterator end = test.end();

    std::cerr << "TEST: |" << test << "| ";
    uri::ipv6_address<string::const_iterator> grammar;

    std::string parsed;
    phrase_parse(begin, end, grammar, space, parsed);

    bool pass = parsed == test;
    if (pass) {
        std::cerr << "good (" << parsed << " == " << test << ")" << std::endl;
    }
    else {
        std::cerr << "bad (" << parsed << " != " << test << ")" << std::endl;
    }

    return pass;
}

BOOST_AUTO_TEST_CASE(ipv6_addresses)
{

    BOOST_CHECK(true  == P("1080:0:0:0:8:800:200C:417A"));
    BOOST_CHECK(true  == P("1080::8:800:200C:417A"));

    BOOST_CHECK(true  == P("FF01:0:0:0:0:0:0:101"));
    BOOST_CHECK(true  == P("FF01::101"));

    BOOST_CHECK(true  == P("0:0:0:0:0:0:0:1"));
    BOOST_CHECK(true  == P("::1"));

    BOOST_CHECK(true  == P("0:0:0:0:0:0:0:0"));
    BOOST_CHECK(true  == P("::"));

    BOOST_CHECK(true  == P("1:0:0:0:0:0:0:0"));
    BOOST_CHECK(true  == P("1::"));

    BOOST_CHECK(true  == P("0:0:0:0:0:0:13.1.68.3"));
    BOOST_CHECK(true  == P("::13.1.68.3"));

    BOOST_CHECK(true  == P("0:0:0:0:0:FFFF:129.144.52.38"));
    BOOST_CHECK(true  == P("::FFFF:129.144.52.38"));

    BOOST_CHECK(true  == P("1:2:3:4:5:6:7:8"));
    BOOST_CHECK(false == P("1:2:3:4:5:6:7:8:9"));
    BOOST_CHECK(false == P("1::2:3:4:5:6:7:8"));
    BOOST_CHECK(true  == P("1::2:3:4:5:6:7"));
    BOOST_CHECK(true  == P("1::2:3:4:5:6"));
    BOOST_CHECK(true  == P("1::2:3:4:5"));
    BOOST_CHECK(true  == P("1::2:3:4"));
    BOOST_CHECK(true  == P("1::2:3"));
    BOOST_CHECK(true  == P("1::2"));

    BOOST_CHECK(true  == P("1:2::3"));
    BOOST_CHECK(true  == P("1:2:3::4"));
    BOOST_CHECK(true  == P("1:2:3:4::5"));
    BOOST_CHECK(true  == P("1:2:3:4:5::6"));
    BOOST_CHECK(true  == P("1:2:3:4:5:6::7"));
    BOOST_CHECK(false == P("1:2:3:4:5:6:7::8"));

    BOOST_CHECK(true  == P("1:2::3:4:5:6:7"));
    BOOST_CHECK(true  == P("1:2:3::4:5:6:7"));
    BOOST_CHECK(true  == P("1:2:3:4::5:6:7"));
    BOOST_CHECK(true  == P("1:2:3:4:5::6:7"));
    BOOST_CHECK(true  == P("1:2:3:4:5:6::7"));
    BOOST_CHECK(false == P("1:2:3:4:5:6:7::8"));
}


//BOOST_AUTO_TEST_SUITE_END()

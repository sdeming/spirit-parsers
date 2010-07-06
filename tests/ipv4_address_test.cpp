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
    uri::ipv4_address<string::const_iterator> grammar;

    std::string parsed;
    bool pass = phrase_parse(begin, end, grammar, space, parsed);
    if (pass) {
        std::cerr << "good, parsed as: " << parsed << std::endl;
    }
    else {
        std::cerr << "bad" << std::endl;
    }

    return pass;
}

BOOST_AUTO_TEST_CASE(ipv4_addresses)
{
    BOOST_CHECK(false == P("."));
    BOOST_CHECK(false == P(".."));
    BOOST_CHECK(false == P("..."));

    BOOST_CHECK(true  == P("0.0.0.0"));
    BOOST_CHECK(true  == P("00.00.00.00"));
    BOOST_CHECK(true  == P("000.000.000.000"));
    BOOST_CHECK(false == P("0000.000.000.000"));
    BOOST_CHECK(false == P("000.0000.000.000"));
    BOOST_CHECK(false == P("000.000.0000.000"));
    BOOST_CHECK(false == P("000.000.000.0000"));

    BOOST_CHECK(true  == P("255.0.0.0"));
    BOOST_CHECK(true  == P("255.255.0.0"));
    BOOST_CHECK(true  == P("255.255.255.0"));
    BOOST_CHECK(true  == P("255.255.255.255"));
    BOOST_CHECK(false == P("256.255.255.255"));
    BOOST_CHECK(false == P("255.256.255.255"));
    BOOST_CHECK(false == P("255.255.256.255"));
    BOOST_CHECK(false == P("255.255.255.256"));

    BOOST_CHECK(true   == P("199.255.39.49"));
    BOOST_CHECK(false  == P("199.256.39.49"));

    BOOST_CHECK(false == P("1"));
    BOOST_CHECK(false == P("1.2"));
    BOOST_CHECK(false == P("1.2.3"));
    BOOST_CHECK(true  == P("1.2.3.4"));
    BOOST_CHECK(true  == P("01.02.03.04"));
    BOOST_CHECK(true  == P("001.002.003.004"));
    BOOST_CHECK(true  == P("10.20.30.40"));
    BOOST_CHECK(true  == P("100.120.130.140"));
    BOOST_CHECK(true  == P("100.220.230.240"));
    BOOST_CHECK(false == P("100.320.230.240"));
    BOOST_CHECK(false == P("100.220.330.240"));
    BOOST_CHECK(false == P("100.220.230.340"));

    BOOST_CHECK(false  == P("a1.2.3.4"));
    BOOST_CHECK(false  == P("1a.2.3.4"));
    BOOST_CHECK(false  == P("1.a2.3.4"));
    BOOST_CHECK(false  == P("1.2a.3.4"));
    BOOST_CHECK(false  == P("1.2.a3.4"));
    BOOST_CHECK(false  == P("1.2.3a.4"));
    BOOST_CHECK(false  == P("1.2.3.a4"));
    BOOST_CHECK(true   == P("1.2.3.4a"));

    BOOST_CHECK(false  == P("1000.2.3.4"));
    BOOST_CHECK(false  == P("1.2000.3.4"));
    BOOST_CHECK(false  == P("1.2.300.4"));
    BOOST_CHECK(false  == P("1.2.3000.4"));
    BOOST_CHECK(false  == P("1.2.3.400"));
    BOOST_CHECK(false  == P("1.2.3.4000"));
}


//BOOST_AUTO_TEST_SUITE_END()

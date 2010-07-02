#define BOOST_TEST_MODULE ParserTest
#include <boost/test/unit_test.hpp>

#include "uri_parser.h"

//BOOST_AUTO_TEST_SUITE(test_suite)

template <typename T>
bool P(uri::uri_type &uri, const T &test)
{
    using boost::spirit::ascii::space;
    using boost::spirit::qi::phrase_parse;

    typename T::const_iterator begin = test.begin();
    typename T::const_iterator end = test.end();

    uri::uri_type new_uri;
    uri = new_uri;
    
    std::cerr << "TEST: |" << test << "|" << std::endl << "==================" << std::endl;
    uri::uri_parser<typename T::const_iterator> grammar(uri);
    bool pass = phrase_parse(begin, end, grammar, space);
    std::cerr << uri.to_string() << std::endl; 
    std::cerr << "==================" << std::endl;
    return pass;
}

bool P(uri::uri_type &uri, const char *test)
{
    return P(uri, std::string(test));
}

BOOST_AUTO_TEST_CASE(uri_components)
{
    uri::uri_type uri;
    BOOST_CHECK(true == P(uri, "http://www.makefile.com"));
}


//BOOST_AUTO_TEST_SUITE_END()

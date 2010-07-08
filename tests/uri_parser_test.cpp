#define BOOST_TEST_MODULE ParserTest
#include <boost/test/unit_test.hpp>

#include "uri_parser.h"

//BOOST_AUTO_TEST_SUITE(test_suite)

template <typename T>
bool P(uri::uri_t &uri, const T &test)
{
    using boost::spirit::ascii::space;
    using boost::spirit::qi::phrase_parse;

    typename T::const_iterator begin = test.begin();
    typename T::const_iterator end = test.end();

    uri::uri_t new_uri;
    uri = new_uri;
    
    std::cerr << "TEST: |" << test << "|" << std::endl;
    uri::uri_parser<typename T::const_iterator> grammar(uri);
    bool pass = phrase_parse(begin, end, grammar, space);
    std::cerr << uri.to_string() << std::endl; 
    std::cerr << "=====================================" << std::endl;
    return pass;
}

bool P(uri::uri_t &uri, const char *test)
{
    return P(uri, std::string(test));
}

BOOST_AUTO_TEST_CASE(uri_pct_encoded_host)
{
    uri::uri_t uri;
    BOOST_CHECK(true == P(uri, "http://%6d%61%6b%65%66%69%6c%65%2e%63%6f%6d"));
    BOOST_CHECK("makefile.com" == uri.host);
}

BOOST_AUTO_TEST_CASE(uri_components)
{
    uri::uri_t uri;
    BOOST_CHECK(true == P(uri, "http://makefile.com"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://makefile.com"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://makefile.com#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://makefile.com:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@makefile.com:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com:443#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com/"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com/#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/deeper/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/deeper/path#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://makefile.com:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@makefile.com:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com/?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/deeper/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://makefile.com:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@makefile.com:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@makefile.com/?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@makefile.com:8080/deeper/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("makefile.com"  == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);
}

BOOST_AUTO_TEST_CASE(uri_components_with_ipv4_addresses)
{
    uri::uri_t uri;
    BOOST_CHECK(true == P(uri, "http://10.0.0.1"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://10.0.0.1"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://10.0.0.1#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://10.0.0.1:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@10.0.0.1:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1:443#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1/"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1/#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/deeper/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/deeper/path#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://10.0.0.1:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@10.0.0.1:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1/?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/deeper/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://10.0.0.1:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@10.0.0.1:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@10.0.0.1/?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@10.0.0.1:8080/deeper/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);
}

BOOST_AUTO_TEST_CASE(uri_components_with_ipv6_v4_mixed_addresses)
{
    uri::uri_t uri;
    BOOST_CHECK(true == P(uri, "http://[FFFF:1:2::10.0.0.1]"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://[FFFF:1:2::10.0.0.1]"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://[FFFF:1:2::10.0.0.1]#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://[FFFF:1:2::10.0.0.1]:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@[FFFF:1:2::10.0.0.1]:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]:443"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]:443#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]/"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]/#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/deeper/path"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/deeper/path#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK(""              == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]/?foo=1&goo=2"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/deeper/path?foo=1&goo=2"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK(""              == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK(""              == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe@[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe"           == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]:443?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("443"           == uri.port);
    BOOST_CHECK(""              == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "https://joe:letmein@[FFFF:1:2::10.0.0.1]/?foo=1&goo=2#frag"));
    BOOST_CHECK("https"         == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK(""              == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/"             == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/path"         == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);

    BOOST_CHECK(true == P(uri, "http://joe:letmein@[FFFF:1:2::10.0.0.1]:8080/deeper/path?foo=1&goo=2#frag"));
    BOOST_CHECK("http"          == uri.scheme);
    BOOST_CHECK("joe:letmein"   == uri.user_info);
    BOOST_CHECK("FFFF:1:2::10.0.0.1"      == uri.host);
    BOOST_CHECK("8080"          == uri.port);
    BOOST_CHECK("/deeper/path"  == uri.path);
    BOOST_CHECK("foo=1&goo=2"   == uri.query);
    BOOST_CHECK("frag"          == uri.fragment);
}



//BOOST_AUTO_TEST_SUITE_END()

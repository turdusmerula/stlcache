//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE "STLCachePolicyLFUAging*"

#ifndef _MSC_VER
#include <unistd.h>
#define WAIT_A_SECOND sleep(2);
#else
#include <windows.h>
#define WAIT_A_SECOND Sleep(2000);
#endif /* _MSC_VER */

#include <boost/test/unit_test.hpp>

#include <stlcache/stlcache.hpp>

using namespace stlcache;
using namespace std;

BOOST_AUTO_TEST_SUITE(STLCacheSuite)

BOOST_AUTO_TEST_CASE(lastInserted) {
    cache<int,string,policy_lfuagingstar<3600> > c1(3);

    c1.insert(1,"data1");
    c1.insert(2,"data2");
    c1.insert(3,"data3");
    c1.insert(4,"data4");

    BOOST_CHECK(c1.size()==3);
}

BOOST_AUTO_TEST_CASE(touch) {
    cache<int,string,policy_lfuagingstar<3600> > c1(3);

    c1.insert(1,"data1");
    c1.insert(2,"data2");
    c1.insert(3,"data3");

    c1.touch(1);
    c1.touch(2);

    c1.insert(4,"data4");

    BOOST_REQUIRE_THROW(c1.fetch(3),exception_invalid_key); //Must be removed by LFU* policy (cause 1&2 are touched)
}

BOOST_AUTO_TEST_CASE(veryfrequent) {
    cache<int,string,policy_lfuagingstar<3600> > c1(3);

    c1.insert(1,"data1");
    c1.insert(2,"data2");
    c1.insert(3,"data3");

    c1.touch(1);
    c1.touch(2);
    c1.touch(3);

    BOOST_REQUIRE_THROW(c1.insert(4,"data4"),exception_cache_full); //Because every entry in cache have reference counter bigger then one and 
                                                                                                    //lfu* policy works only on entries with refcount equal to 1

    c1.erase(1);
    BOOST_REQUIRE_NO_THROW(c1.insert(4,"data4"));

    BOOST_REQUIRE_THROW(c1.fetch(1),exception_invalid_key);
    BOOST_REQUIRE_NO_THROW(c1.fetch(4));
}

BOOST_AUTO_TEST_CASE(expire) {
    cache<int,string,policy_lfuagingstar<1> > c1(3);

    c1.insert(1,"data1");
    c1.insert(2,"data2");
    c1.insert(3,"data3");

    c1.touch(1); //For key one refcount is 3 now
    c1.touch(1); 
    c1.touch(2); //For key two refcount is 3 now
    c1.touch(2);
    c1.touch(3); //For key three refcount is 2 now

    WAIT_A_SECOND;

    c1.insert(4,"data4");

    BOOST_REQUIRE_THROW(c1.fetch(3),exception_invalid_key); //Must be removed by LFU policy (cause every item have been touched and refcount for key 3 is 1)
}

BOOST_AUTO_TEST_CASE(expirefail) {
    cache<int,string,policy_lfuagingstar<1> > c1(3);

    c1.insert(1,"data1");
    c1.insert(2,"data2");
    c1.insert(3,"data3");

    c1.touch(1); //For key one refcount is 4 now
    c1.touch(1); 
    c1.touch(1);
    c1.touch(2); //For key two refcount is 4 now
    c1.touch(2);
    c1.touch(2);
    c1.touch(3); //For key three refcount is 3 now
    c1.touch(3);

    WAIT_A_SECOND;

    BOOST_REQUIRE_THROW(c1.insert(4,"data4"),exception_cache_full); //Cause even in expired cache there are no entries with refcount 1 

    WAIT_A_SECOND;

    BOOST_REQUIRE_NO_THROW(c1.insert(4,"data4"));
    BOOST_REQUIRE_THROW(c1.fetch(3),exception_invalid_key); //Must be removed by LFU policy (cause every item have been expired again and refcount for key 3 is 1)
}
BOOST_AUTO_TEST_SUITE_END();

//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE "STLCacheVictimPerformance"
#include <boost/test/unit_test.hpp>

#ifdef _WIN32
#include "timeval.h"
#else 
#include <sys/time.h>
#endif /* _WIN32 */
#include <iostream>
#include <stlcache/stlcache.hpp>

using namespace stlcache;
using namespace std ;

BOOST_AUTO_TEST_SUITE(STLCacheSuite)

const unsigned long noItems = 65536;

BOOST_AUTO_TEST_CASE(victimNone) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_none> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_none cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimLRU) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_lru> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_lru cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimUnorderedLRU) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_unordered_lru> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL);

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL);

    cout<<"Insertion of "<<noItems<<" excessive items into policy_unordered_lru cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimMRU) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_mru> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_mru cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimLFU) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_lfu> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_lfu cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimLFUStar) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_lfustar> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_lfustar cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimLFUAging) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_lfuaging<3600> > c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_lfuaging cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(victimLFUAgingStar) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_lfuagingstar<3600> > c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_lfuagingstar cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_CASE(insertAdaptive) {
    struct timeval start,stop;

    cache<unsigned int,unsigned int,policy_adaptive> c(noItems);
    for(unsigned int indx = 0; indx<noItems; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&start, NULL); 

    for(unsigned int indx = noItems; indx<noItems*2; indx++) {
        c.insert(indx,indx);
    }

    gettimeofday(&stop, NULL); 

    cout<<"Insertion of "<<noItems<<" excessive items into policy_adaptive cache took "<<((stop.tv_sec-start.tv_sec)*1000)+((stop.tv_usec-start.tv_usec)/1000)<<" milliseconds"<<endl;
}

BOOST_AUTO_TEST_SUITE_END();

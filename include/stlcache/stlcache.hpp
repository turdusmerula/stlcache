//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_STLCACHE_HPP_INCLUDED
#define STLCACHE_STLCACHE_HPP_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif /* _MSC_VER */

#include <map>

#include <stlcache/exceptions.hpp>
#include <stlcache/policy.hpp>
#include <stlcache/policy_lru.hpp>
#include <stlcache/policy_mru.hpp>
#include <stlcache/policy_lfu.hpp>
#include <stlcache/policy_lfustar.hpp>
#include <stlcache/policy_lfuaging.hpp>
#include <stlcache/policy_lfuagingstar.hpp>
#include <stlcache/policy_adaptive.hpp>

#include <stlcache/cache_map.hpp>
#include <stlcache/cache_multimap.hpp>
#include <stlcache/cache_unordered_map.hpp>
#include <stlcache/cache_unordered_multimap.hpp>

namespace stlcache {

    /*! 
      \mainpage STL::Cache - in-memory cache for C++ applications
     
      \section Introduction
     
     STL::Cache is just a simple wrapper over standard map, that implements some cache algorithms, thus allowing you to limit the storage size
     and automatically remove unused items from it. (Right now the std::map interface is partially implemented, so it can't be used as a drop-in replacement for the std::map).
     
     It is intented to be used for keeping any key/value data, especially when data's size are too big, to just put it into the map and keep the whole thing.
     With STL::Cache you could put unlimited (really unlimited) amount of data into it, but it will store only some small part of your data. So re-usable
     data will be kept near your code and not so popular data will not spend expensive memory. 
     
     STL::Cache uses configurable policies, for decisions, whether data are good, to be kept in cache or they should be thrown away. It is shipped with
     8 policies and you are free to implement your own.        
     
     \section GaI Getting and installing
     
     The STL::Cache source code is hosted on <a href="https://github.com/akashihi/stlcache">github</a>, so you can download the source code
     using wget or other download tool:
     
     \li https://github.com/akashihi/stlcache/tarball/master (tar.gz format)
     \li https://github.com/akashihi/stlcache/zipball/master (or zip format)
     
     You could also clone the repository and check some unstable code:
     
     \code
     git clone git://github.com/akashihi/stlcache.git
     \endcode
     
     The 'master' branch is always pointing to the latest stable release and the 'next' branch is a unstable development branch.
     
     The STL::Cache is header only and does not require any building. Just copy the include/stlcache directory to your system's include directories or
     add it to your project's  include directories. Alternativaly you could build the sources and then install it, using our build system.
     
     Yes, as i told you, the STL::Cache themself is a header-only library and doesn't requires building. Indeed, it is shipped with tests, documentation and
     other stuff, that could be built and used. For STL::Cache building you need:
     
     \li <a href="http://www.cmake.org/">Recent CMake (required)</a>
     \li <a href="http://www.boost.org/">Boost library (optional, used only for tests)</a>
     \li <a href="http://www.doxygen.org/">Doxygen (optional, only for documentation processing)</a>
     
     After getting this stuff up and running, select a directory for building and issue the following commands:
     
     \code
        cmake /path/to/the/stlcache/sources
        make
        make test
        make doc
        make install
    \endcode
     
     The CMake is using a <a href="http://www.cmake.org/Wiki/CMake_FAQ#Out-of-source_build_trees">out-of-source</a> build approach,
     so it is recommended to create a temporary build directory somewhere and remove it after installation.
     
     The generated documentation will be put into the \<build\>/doc directory and tests will be built and run directly in the \<build\> directory. They are NOT
     installed by 'make install' command, so you have to copy them manually, if you really need them.
     
     \section Usage
     
     Select a \link stlcache::policy cache expiration policy \endlink, configure \link stlcache::cache cache \endlink with it, construct the cache, specifying it's size, and start putting data into the cache:
     
     \code
     typedef CacheLRU stlcache::cache<int,string,stlcache::policy_lru>;
     CacheLRU myCache(10);
     \endcode
     
     The policy, key data type and value data type are passed as parameters to the \link stlcache::cache \endlink class template. In the example above
     we have constructed cache, that keeps data of type std::string and keys are of type int. It uses a \link stlcache::policy_lru LRU \endlink policy
     for removal of excessive items. Cache object 'myCache' is able to keep only 10 items. Cache size is configured at the construction time and
     cannot be changed in the future (with exceptions for assignment and swap operations). Additionally you could specify a comparision object and
     allocator type, see the \link stlcache::cache cache class \endlink documentation for the details.
     
     You could put objects into your cache object using \link cache::insert insert \endlink call:
     
     \code
     myCache.insert(1,"One)";
     \endcode
     
     Note, that the \link cache::insert insert \endlink call could throw a \link stlcache::exception_cache_full cache full exception \endlink or
     \link stlcache::exception_invalid_key invalid key exception \endlink As keys have to be unique, the insert call may do nothing, if it meets the
     duplicate key. Don't forget to check it's return value.
     
     Now, when you have some data in the cache, you may want to retrieve it back:
     
     \code
     string myOne = myCache.fetch(1);
     \endcode
     
     The \link cache::fetch fetch \endlink call will return a data value, associated with the supplied key or throw the \link stlcache::exception_invalid_key invalid key exception \endlink
     when the key doesn't exists in the cache. For safier operations, you could \link cache::check check \endlink in advance, whether key is in the cache or not:
     \code
     string myOne;
     if (myCache.check(1)) {
       myOne = myCache.fetch(1);
     }
     \endcode
     
     The \link cache::check check \endlink is exception-safe.
     
     Both \link cache::check check \endlink and \link cache::fetch fetch \endlink calls are increasing internal reference count for the key, so, depending
     on the used policy, it will increase or decrease the entry's chance to become an expiration victim. Under some circumstances you may
     need to manually change those reference counter, without actually accesing the entry. Something like \link cache::touch touching \endlink it:
     \code
     myCache.touch(1);
     \endcode
     
     The \link cache::touch \endlink call is exception-safe and could be used even on non-existent keys.
     
     When you have done your work, you may manually delete the entry:
     
     \code
     myCache.erase(1);
     \endcode
     
     Check it's return value, to get sure, whether something was deleted or not.
     
     \section Policies
     
     The \link stlcache::policy policy \endlink is a pluggable implementation of a cache algorithms, used for finding and removing excessive
     cache entries. STL::Cache is shipped with the following policies:
     
     \li \link stlcache::policy_none None \endlink - A random expiration policy. Removes some random entry on request
     \li \link stlcache::policy_lru LRU \endlink - 'Least recently used' policy.
     \li \link stlcache::policy_mru MRU \endlink - 'Most recentrly used' policy
     \li \link stlcache::policy_lfu LFU \endlink - 'Least frequently used' policy
     \li \link stlcache::policy_lfustar LFU* \endlink - 'Least frequently used' polcy, that expires only items with refcount 1, as proposed by M.Arlitt
     \li \link stlcache::policy_lfuaging LFU-Aging \endlink - 'Least frequently used' policy with time-based decreasing of usage count
     \li \link stlcache::policy_lfuagingstar LFU*-Aging \endlink - Combination of \link stlcache::policy_lfustar LFU* \endlink and \link stlcache::policy_lfuagingstar LFU*-Aging \endlink policies
     \li \link stlcache::policy_adaptive Adaptive Replacement \endlink - 'Adaptive Replacement' policy
     
     The cache expiration policy must be specified as a third parameter of \link stlcache::cache cache \endlink type and it is mandatory.
     
     \section Writing your own policy
     
     The policy implementation should keep track of entries in the cache and it must be able to tell the cache, what item should be expired at the moment.
     There is not any limitations on policy internal structure and stuff, but it is expected, that policy conforms to some predefined interfaces.
     
     First of all - every policy is built in two classes, one class is the policy iteslf, and another one is a 'bind wrapper':
     
     \note All code examples in this section are from \link stlcache::policy_none  policy none \endlink
     
     \code
        struct policy_none {
        template <typename Key, template <typename T> class Allocator>
            struct bind : _policy_none_type<Key,Allocator> { 
                bind(const bind& x) : _policy_none_type<Key,Allocator>(x)  { }
                bind(const size_t& size) : _policy_none_type<Key,Allocator>(size) { }
            };
        };
     \endcode
     
     As you may see, the policy itself is automatically configured with caches's Key type and Allocator type. Of course, you could also pass
     your own template parameters and partially instantiate the policy implementation template:
     
     \code
        template <class R> struct policy_none {
            template <typename Key, template <typename T> class Allocator>
                struct bind : _policy_none_type<R,Key,Allocator> { 
                    bind(const bind& x) : _policy_none_type<R,Key,Allocator>(x) { }
                    bind(const size_t& size) : _policy_none_type<R,Key,Allocator>(size) { }
                };
        };     
     \endcode
     
     You could pass some implementation of R during cache type definition:
     
     \code
         stlcache::cache<int,int,stlcache::policy_none<Randomizer> > c;
     \endcode
     
     Well, the actual implementation must implement the \link stlcache::policy policy interface \endlink :
     
     \code
        template <class Key,template <typename T> class Allocator> class policy {
            public:
            virtual void insert(const Key& _k) throw(exception_invalid_key) =0;
            virtual void remove(const Key& _k) throw() =0;
            virtual void touch(const Key& _k) throw() =0;
            virtual void clear() throw() =0;
            virtual void swap(policy<Key,Allocator>& _p) throw(exception_invalid_policy)=0;
            virtual const _victim<Key> victim() throw()  =0;
        };     
     \endcode
     
     So, the policy could be asked for a \link policy::victim victim \endlink, entries could be \link policy::insert inserted \endlink ,
     \link policy::remove removed \endlink and \link policy::touch touched \endlink. It's contents could also be \link policy::clear cleared \endlink
     or \link policy::swap swapped \endlink with another policy. Concrete policy implementation should be CopyConstructible, Assignable and
     must provide a constructor, for specifiying policy size:
     
     \code
        template <class Key, template <typename T> class Allocator> class _policy_none_type : public policy<Key,Allocator> {
        public:
            _policy_none_type<Key,Allocator>& operator= ( const _policy_none_type<Key,Allocator>& x) throw() { }
            _policy_none_type(const _policy_none_type<Key,Allocator>& x) throw() {}
            _policy_none_type(const size_t& size ) throw() { }

            virtual void insert(const Key& _k) throw(exception_invalid_key) {}
            virtual void remove(const Key& _k) throw() {}
            virtual void touch(const Key& _k) throw() {}
            virtual void clear() throw() {}
            virtual void swap(policy<Key,Allocator>& _p) throw(exception_invalid_policy) {}

            virtual const _victim<Key> victim() throw() {}
        };
     \endcode
     
     It's up to you, how you will implement those methods and so on. The only importatn thing, we haven't mentioned yet, is a
     \link stlcache::_victim victim \endlink class. It is a way to return optional value from a function. So, when your policy implementatiton
     cannot find any entry to remove, it will return empty victim object.
     
     \section AaL  Authors and Licensing
     
     Copyright (C) 2011 Denis V Chapligin
     Distributed under the Boost Software License, Version 1.0.
     (See accompanying file LICENSE_1_0.txt or copy at
     http://www.boost.org/LICENSE_1_0.txt)
     
      */

}

#endif /* STLCACHE_STLCACHE_HPP_INCLUDED */

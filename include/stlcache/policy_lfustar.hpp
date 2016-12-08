//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_POLICY_LFUSTAR_HPP_INCLUDED
#define STLCACHE_POLICY_LFUSTAR_HPP_INCLUDED

#include <set>
#include <map>

#include <stlcache/policy.hpp>

namespace stlcache {
    template <class Key,template <typename T> class Container> class _policy_lfustar_type : public virtual _policy_lfu_type<Key,Container> {
        using keySetType = typename Container<Key>::LFUStarKeySetType ;
        keySetType keySet;
    public:
        _policy_lfustar_type(const size_t& size ) throw() : _policy_lfu_type<Key,Container>(size) { }
        _policy_lfustar_type(const _policy_lfustar_type& x ) throw() : _policy_lfu_type<Key,Container>(x) { }

        virtual const _victim<Key> victim() throw()  {
            //LFU* only operates on entries with references count equal to 1
            typedef typename _policy_lfu_type<Key,Container>::entriesType::const_iterator entriesIterType;
            entriesIterType entriesIter = this->entries().find(1);
            if (entriesIter==this->entries().end()) {
                return _victim<Key>();
            }

            return _victim<Key>(entriesIter->second);
        }
    };

    template <class Key>
    struct lfustar_default_container : public lfu_default_container<Key>
    {
    	using LFUStarKeySetAllocator = std::allocator<Key> ;
    	using LFUStarKeySetType = std::set<Key, std::less<Key>, LFUStarKeySetAllocator> ;
    } ;

    /*!
     * \brief A 'LFU*' policy
     * 
     * Implements <a href="http://en.wikipedia.org/wiki/Least_frequently_used">'LFU*'</a> cache algorithm. 
     *  
     * A modified \link stlcache::policy_lfu LFU \endlink policy implementation, that expires onty items with usage count equal to 1. 
     * In other words, this policy still tracks, how much times the entry were used, but selects for the expiration only those entries, that 
     * was touched not more, then one time. 
     *  
     * For example, when you put a entry into the cache and then never use it 
     * (either by \link cache::fetch fetching \endlink or  \link cache::touch touching \endlink it), the entry may be expired sometime. 
     * If you only \link cache::touch touch \endlink of \link cache::fetch fetch \endlink the item once, it still will be a candidate for expiration. 
     * But. If you use there entry one more time (at least tow times), it will be never removed from the cache automatically and must be 
     * erased by hand. 
     *   
     * So, \link cache::touch Touching \endlink the entry twice makes entry unexpirable. Because of this, the LFU* policy may not be able 
     * to find a excessive entry, when cache is full, so the \link cache::insert insert call \endlink may throw a \link stlcache::exception_cache_full 
     * cache full \endlink exception.
     *  
     * No additional configuration is required. 
     *  
     * \see policy_lfu 
     * \see policu_lfuagingstar 
     */
    struct policy_lfustar {
        template <typename Key>
            struct bind : _policy_lfustar_type<Key, lfustar_default_container> {
                bind(const bind& x) : _policy_lfustar_type<Key,lfustar_default_container>(x),_policy_lfu_type<Key,lfustar_default_container>(x)  { }
                bind(const size_t& size) : _policy_lfustar_type<Key,lfustar_default_container>(size),_policy_lfu_type<Key,lfustar_default_container>(size) { }
            };
    };
}

#endif /* STLCACHE_POLICY_LFUSTAR_HPP_INCLUDED */

//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_POLICY_LRU_HPP_INCLUDED
#define STLCACHE_POLICY_LRU_HPP_INCLUDED

#include <unordered_map>

#include <stlcache/policy.hpp>

namespace stlcache {
    template <class Key, template <typename T> class Container>
    class _policy_lru_type : public policy<Key> {
        using entriesList = typename Container<Key>::entriesType ;
        using entriesIterator = typename entriesList::iterator ;
        entriesList _entries;

        using map = typename Container<Key>::entriesMap ;
        using entriesMapIterator = typename map::iterator ;
        map _entriesMap ;
    public:
        _policy_lru_type<Key,Container>& operator= ( const _policy_lru_type<Key,Container>& x) throw() {
            this->_entries=x._entries;
            return *this;
        }
        _policy_lru_type(const _policy_lru_type<Key,Container>& x) throw() {
            *this=x;
        }
        _policy_lru_type(const size_t& size ) throw() { }

        virtual void insert(const Key& _k) throw(exception_invalid_key) {
            entriesIterator entryIter = _entries.insert(_entries.begin(),_k);
            _entriesMap.insert(std::pair<Key,entriesIterator>(_k,entryIter));
        }
        virtual void remove(const Key& _k) throw() {
            entriesMapIterator mapIter = _entriesMap.find(_k);
            if (mapIter==_entriesMap.end()) {
                return;
            }
            _entries.erase(mapIter->second);
            _entriesMap.erase(mapIter);
        }
        virtual void touch(const Key& _k) throw() { 
            entriesMapIterator mapIter = _entriesMap.find(_k);
            if (mapIter==_entriesMap.end()) {
                return;
            }
            _entries.erase(mapIter->second);
            entriesIterator entryIter = _entries.insert(_entries.begin(),_k);
            mapIter->second=entryIter;
        }
        virtual void clear() throw() {
            _entries.clear();
        }
        virtual void swap(policy<Key>& _p) throw(exception_invalid_policy) {
            try {
                _policy_lru_type<Key,Container>& _pn=dynamic_cast<_policy_lru_type<Key,Container>& >(_p);
                _entries.swap(_pn._entries);
                _entriesMap.swap(_pn._entriesMap);
            } catch (const std::bad_cast& ) {
                throw exception_invalid_policy("Attempted to swap incompatible policies");
            }
        }

        virtual const _victim<Key> victim() throw()  {
            return _victim<Key>(_entries.back());
        }

    protected:
        const entriesList& entries() const  { return this->_entries; }
    };

    template <class Key>
    struct lru_default_container
    {
    	using entriesAllocator = std::allocator<Key> ;
    	using entriesType = std::list<Key, entriesAllocator> ;
    	using entriesIterator = typename entriesType::iterator ;

    	using entriesMapAllocator = std::allocator<std::pair<const Key, entriesIterator>> ;
    	using entriesMap = std::map<Key, entriesIterator, std::less<Key>, entriesMapAllocator> ;
    	using entriesMapIterator = typename entriesMap::iterator ;
    } ;

    /*!
     * \brief A 'Least Recently Used' policy
     * 
     * Implements <a href="http://en.wikipedia.org/wiki/Cache_algorithms#Least_Recently_Used">'Least Recently Used'</a> cache expiration algorithm. 
     *  
     * The LRU policy tracks the usage of items and moves the recently used items to the front of the items stack and select items for the expiration from the end 
     * of the stack. \link cache::touch Touching \endlink the entry decreases item's expiration probability. This policy is always able to expire any amount of entries. 
     *  
     * No additional configuration is required. 
     */
    struct policy_lru {
        template <typename Key>
            struct bind : _policy_lru_type<Key,lru_default_container> {
                bind(const bind& x) : _policy_lru_type<Key,lru_default_container>(x)  { }
                bind(const size_t& size) : _policy_lru_type<Key,lru_default_container>(size) { }
            };
    };

    template <class Key>
    struct lru_unordered_map_container
    {
    	using entriesAllocator = std::allocator<Key> ;
    	using entriesType = std::list<Key, entriesAllocator> ;
    	using entriesIterator = typename entriesType::iterator ;

    	using entriesMapAllocator = std::allocator<std::pair<const Key, entriesIterator>> ;
    	using entriesMap = std::unordered_map<Key, entriesIterator, std::hash<Key>, std::equal_to<Key>, entriesMapAllocator> ;
    	using entriesMapIterator = typename entriesMap::iterator ;
    } ;

    struct policy_unordered_lru {
        template <typename Key>
            struct bind : _policy_lru_type<Key,lru_unordered_map_container> {
                bind(const bind& x) : _policy_lru_type<Key,lru_unordered_map_container>(x)  { }
                bind(const size_t& size) : _policy_lru_type<Key,lru_unordered_map_container>(size) { }
            };
    };
}

#endif /* STLCACHE_POLICY_LRU_HPP_INCLUDED */

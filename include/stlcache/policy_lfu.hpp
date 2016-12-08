//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_POLICY_LFU_HPP_INCLUDED
#define STLCACHE_POLICY_LFU_HPP_INCLUDED

#include <set>
#include <map>

#include <stlcache/policy.hpp>

namespace stlcache {
    template <class Key,template <typename T> class Container> class _policy_lfu_type : public policy<Key> {
    protected:
        using entriesPair = typename Container<Key>::LFUEntriesPair ;
        using entriesType = typename Container<Key>::LFUEntriesType ;
        using entriesIterator = typename entriesType::iterator ;
        entriesType _entries;

        using backEntriesPair =  typename Container<Key>::LFUBackEntriesPair;
        using backEntriesType = typename Container<Key>::LFUBackEntriesType;
        using backEntriesIterator=typename backEntriesType::iterator;
        backEntriesType _backEntries;
        
    public:
        _policy_lfu_type<Key,Container>& operator= ( const _policy_lfu_type<Key,Container>& x) throw() {
            this->_entries=x._entries;
            this->_backEntries=x._backEntries;
            return *this;
        }
        _policy_lfu_type(const _policy_lfu_type<Key,Container>& x) throw() {
            *this=x;
        }
        _policy_lfu_type(const size_t& size ) throw() { }

        virtual void insert(const Key& _k,unsigned int refCount) throw(exception_invalid_key) {
            //1 - is initial reference value
            entriesIterator newEntryIter = _entries.insert(entriesPair(refCount,_k));
            _backEntries.insert(backEntriesPair(_k,newEntryIter));
        }
        virtual void insert(const Key& _k) throw(exception_invalid_key) {
            //1 - is initial reference value
            this->insert(_k,1);
        }

        virtual void remove(const Key& _k) throw() {
            backEntriesIterator backIter = _backEntries.find(_k);
            if (backIter==_backEntries.end()) {
                return;
            }

            _entries.erase(backIter->second);
            _backEntries.erase(_k);
        }
        virtual void touch(const Key& _k) throw() { 
            backEntriesIterator backIter = _backEntries.find(_k);
            if (backIter==_backEntries.end()) {
                return;
            }

            unsigned int refCount=backIter->second->first;
            
            _entries.erase(backIter->second);
            entriesIterator entryIter=_entries.insert(entriesPair(refCount+1,_k));
            backIter->second=entryIter;
        }
        virtual void clear() throw() {
            _entries.clear();
            _backEntries.clear();
        }
        virtual void swap(policy<Key>& _p) throw(exception_invalid_policy) {
            try {
                _policy_lfu_type<Key,Container>& _pn=dynamic_cast<_policy_lfu_type<Key,Container>& >(_p);
                _entries.swap(_pn._entries);
                _backEntries.swap(_pn._backEntries);
            } catch (const std::bad_cast& ) {
                throw exception_invalid_policy("Attempted to swap incompatible policies");
            }
        }

        virtual const _victim<Key> victim() throw()  {
            if (_entries.begin()==_entries.end()) {
                return _victim<Key>();
            }

            return _victim<Key>(_entries.begin()->second);
        }

    protected:
        const entriesType& entries() const {
            return this->_entries;
        }
        virtual unsigned long long untouch(const Key& _k) throw() { 
            backEntriesIterator backIter = _backEntries.find(_k);
            if (backIter==_backEntries.end()) {
                return 0;
            }
            unsigned int refCount=backIter->second->first;


            if (!(refCount>1)) {
                return refCount; //1 is a minimal reference value
            }


            _entries.erase(backIter->second);
            entriesIterator entryIter=_entries.insert(entriesPair(refCount-1,_k));
            backIter->second=entryIter;

            return refCount;
        }
    };

    template <class Key>
    struct lfu_default_container
    {
    	using LFUEntriesPair = std::pair<const unsigned int, Key> ;
    	using LFUEntriesAllocator = std::allocator<LFUEntriesPair> ;
    	using LFUEntriesType = std::multimap<unsigned int, Key, std::less<Key>, LFUEntriesAllocator> ;
        using LFUEntriesIterator = typename LFUEntriesType::iterator ;

        using LFUBackEntriesPair = std::pair<const Key,LFUEntriesIterator> ;
    	using LFUBackEntriesAllocator = std::allocator<LFUBackEntriesPair> ;
    	using LFUBackEntriesType = std::map<Key, LFUEntriesIterator, std::less<Key>, LFUBackEntriesAllocator> ;
    } ;

    /*!
     * \brief A 'Least Frequently Used' policy
     * 
     * Implements <a href="http://en.wikipedia.org/wiki/Least_frequently_used">'Least Frequently Used'</a> cache algorithm. 
     *  
     * The LFU policy tracks how many times entry was used and selects entries with the smallest usage count for expiration. There is a big difference 
     * between LFU and \link stlcache::policy_lru LRU \endlink policies - the LRU policy only tracks the fact of entry usage, but the LFU also takes in
     * the account the number of entry usages. 
     * \link cache::touch Touching \endlink the entry greatly decreases item's expiration probability. This policy is always able to expire any amount of entries. 
     *  
     * No additional configuration is required. 
     *  
     * \see policy_lfustar
     * \see policy_lfuaging 
     * \see policy_lfuagingstar 
     *  
     */
    struct policy_lfu {
        template <typename Key>
            struct bind : _policy_lfu_type<Key,lfu_default_container> {
                bind(const bind& x) : _policy_lfu_type<Key,lfu_default_container>(x)  { }
                bind(const size_t& size) : _policy_lfu_type<Key,lfu_default_container>(size) { }
            };
    };
}

#endif /* STLCACHE_POLICY_LFU_HPP_INCLUDED */

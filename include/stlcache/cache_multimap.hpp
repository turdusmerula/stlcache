//
// Copyright (C) 2011 Denis V Chapligin
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_STLCACHE_MULTIMAP_HPP_INCLUDED
#define STLCACHE_STLCACHE_MULTIMAP_HPP_INCLUDED

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif /* _MSC_VER */

#include <map>

#include <stlcache/exceptions.hpp>

namespace stlcache {

    /*! \brief Cache is a kind of a multimap that have limited number of elements to store and configurable policy for autoremoval of excessive elements.
     *  
     * The cache behaviour resembles a std::map behaviour (but not yet fully compatible with usual std::map interface), but it adds a limitation of 
     * maximum number of entries, stored in cache and  provides a configurable policy, which is used to remove entries when the cache is full. 
     *  
     * Internally it is built as a wrapper over a std::map with some additional checking for cache size and entry removal logic. Unfortunately not all interfaces 
     * of std::map are supperted by now, but it is a work in progress. 
     *  
     * You can use the cache in a simple and obvious way: 
     * \code 
     *     cache<string,string,policy_lru> cache_lru(3); 
     *     cache_lru.insert("key","value");
     *     cache_lru.touch("key");
     *     if (cache_lru.check("key")) {
     *         cout<<"We have some value in the cache: "<<cache_lru.fetch("key");
     *     }
     *     cache_lru.erase("key");
     * \endcode
     *  
     * So you define a cache instance, specifying a Key type, a Data type and a Policy, passing maximum number of entries to the constructor. 
     * You could \link cache::insert insert \endlink as 
     * many entries as you like, even more entries, that cache instance can hold. When the cache starts to overflow, it will automatically 
     * remove entries, depending on specified Policy. 
     * You can \link cache::touch touch \endlink some entries, to advance their usage count, that may (or may not, depending on Policy) 
     * help them survive the removal on cache overflow. You can \link cache::erase \endlink entries manually too. 
     * For the data retrieval you should first \link cache::check check \endlink , whether Key exists in 
     * cache or not, and \link cache::fetch fetch \endlink it, if it exists. 
     * The preliminary \link cache::fetch fetch \endlink step is not neccessary, but fetching non-existent key will throw an \link 	exception_invalid_key exception \endlink . 
     *  
     * cache class have three mandatory template parameters and two optional: 
     * \tparam <Key> The key data type. We have no limitations on this type, so it could be any built-in type, custom class etc. But it is recommended to supply a less-than operation for a Key type. 
     * \tparam <Data> The value data type. You a free to use any type here without any restrictions, requirements and recommendations. 
     * \tparam <Policy> The expiration policy type. Must implement \link stlcache::policy policy interface \endlink STL::Cache provides several expiration policies out-of-box and you are free to define new policies. 
     * \tparam <Compare> Comparison class: A class that takes two arguments of the key type and returns a bool. The expression comp(a,b), where comp is an object of this comparison class and a and b are key values, shall return true if a is to be placed at an earlier position than b in a strict weak ordering operation. This can either be a class implementing a function call operator or a pointer to a function (see constructor for an example). This defaults to less<Key>, which returns the same as applying the less-than operator (a<b). This is required for the underlying std::map 
     * \tparam <Allocator> Type of the allocator object used to define the storage allocation model. Unlike std::map you should pass a unspecialized Allocator type. 
     *  
     * So, the full example of cache instantiation will be: 
     * \code 
     *     cache<int,string,policy_none,less<string>,std::allocator> cache_none(100500);
     * \endcode
     *  
     * \see policy 
     *  
     * \author chollya (5/19/2011)
     */
    template<
        class Key, 
        class Data, 
        class Policy, 
        class Compare = std::less<Key>,
        template <typename T> class Allocator = std::allocator
    >
    class cache_multimap {
		typedef std::multimap<Key,Data,Compare,Allocator<std::pair<const Key, Data> > > storageType;
		typedef typename Policy::template bind<Key> policy_type;

		storageType _storage;
		std::size_t _maxEntries;
		std::size_t _currEntries;
		policy_type* _policy;
		Allocator<policy_type> policyAlloc;

    public:
        /*! \brief The Key type 
         */
        typedef Key                                                                key_type;
        /*!  \brief The Data type 
         */
        typedef Data                                                               mapped_type;
        /*! \brief Combined key,value type 
          */
        typedef std::pair<const Key, Data>                                         value_type;
        /*! \brief Compare type used by this instance
          */
        typedef Compare                                                          key_compare;
        /*! \brief Allocator type used by this instance 
          */
        typedef Allocator<std::pair<const Key, Data> >                          allocator_type;
        /*! \brief Nested class to compare elements (see member function value_comp) 
          */
        typedef typename storageType::value_compare                                value_compare;
        /*! \brief Allocator::reference 
          */
        typedef typename storageType::reference                                        reference;
        /*! \brief Allocator::const_reference 
          */
        typedef typename storageType::const_reference                               const_reference;
        /*! \brief Type used for storing object sizes, specific to a current platform (usually a size_t) 
          */
        typedef typename storageType::size_type                                       size_type;
        /*! \brief Type used for address calculations, specific to a current platform (usually a ptrdiff_t) 
          */
        typedef typename storageType::difference_type                               difference_type;
        /*! \brief Allocator::pointer 
          */
        typedef typename storageType::pointer                                          pointer;
        /*! \brief Allocator::const_pointer 
          */
        typedef typename storageType::const_pointer                                 const_pointer;

        /*! \name std::map interface wrappers 
         *  Simple wrappers for std::map calls, that we are using only for mimicking the map interface
         */
        //@{
        /*! \brief Allocator object accessor
          *                                                                                                                                          
          *  Provides access to the allocator object, used to constuct the container.
          *
          *  \return The allocator object of type Allocator<pair<const Key, Data> >.                
          */
        allocator_type get_allocator() const throw() {
            return _storage.get_allocator();
        }

        /*! \brief Counts entries with specified key in the cache.
          *  
          *  Provides information on number of cache entries with specific key. It is not very usefull, as keys are unique in the cache.
          *  The difference between thic member and \link cache::check check member \endlink is that this one returns number of keys
          *  without touching the entry usage count.
          *  
          *  \param <x> key to count
          *  
          *  \return Number of objects with specified key in the cache (ie 1 if object is on the cache and 0 for non-existent object)
          *
          *  \see check
          *  
          */        
        size_type count ( const key_type& x ) const throw() {
            return _storage.count(x);
        }

        /*! \brief Value comparision object accessor
         *  
         *   Provides access to a comparison object that can be used to compare two element values (pairs) to get whether the key of the first goes before the second. The mapped value, although part of the pair, is not taken into consideration in this comparison - only the key value.  
         *  
         *   \return The value comparison object of type value_compare, defined as described above.
         */
        value_compare value_comp ( ) const throw() {
            return _storage.value_comp();
        }

        /*! \brief Key comparision object accessor 
          * 
          *  Provides access to the  comparison object associated with the container, which can be used to compare the keys of two elements in the container.
          *  This comparison object is set on object construction, and may either be a pointer to a function or an object of a class with a function call operator.
          *  In both cases it takes two arguments of the same type as the element keys,
          *  and returns true if the first argument is considered to go before the second in the strict weak ordering for the map object,
          *  and false otherwise
          *  Notice that the returned comparison object takes as parameters the element keys themselves, not entire element values (pairs).
          *
          *  \return The key comparison object of type key_compare, defined to Compare, which is the fourth template parameter in the map class template.
          */
        key_compare key_comp ( ) const throw() {
            return _storage.key_comp();
        }

        /*! \brief Test whether cache is empty 
          *  
          *  Tells whether the map container is empty, i.e. whether its size is 0. This function does not modify the content of the container in any way. In terms of performance it is not equal to call
          *  \code
          *      cache.size()==0
          *   \endcode
          *  
          *   \return true if the container size is 0, false otherwise.
          *  
          *   \see size
          */
        bool empty() const throw() {
            return _storage.empty();
        }
        //@}

        /*! \name cache api 
         *  members that are specific to cache or implemented with some cache specific things
         */
        //@{
        /*!
         * \brief Clear the cache
         *  
         * Removes all cache entries, drops all usage count data and so on. 
         *  
         * \see size 
         * \see empty 
         *  
         */
        void clear() throw() {
            _storage.clear();
            _policy->clear();
            this->_currEntries=0;
        }
        
        /*!
         * \brief Swaps contents of two caches 
         *  
         * Exchanges the content of the cache with the content of mp, which is another cache object containing elements of the same type and using the same expiration policy. 
         * Sizes may differ. Maximum number of entries may differ too.
         *  
         * \param <mp> Another cache of the same type as this whose cache is swapped with that of this cache. 
         *  
         * \throw <exception_invalid_policy> Thrown when the policies of the caches to swap are incompatible. 
         *  
         * \see cache::operator= 
         */
        void swap ( cache_multimap<Key,Data,Policy,Compare,Allocator>& mp ) throw(exception_invalid_policy) {
            _storage.swap(mp._storage);
            _policy->swap(*mp._policy);

            std::size_t m=this->_maxEntries;
            this->_maxEntries=mp._maxEntries;
            mp._maxEntries=m;

            this->_currEntries=this->size();
            mp._currEntries=mp.size();
        }

        /*!
         * \brief Removes a entry from cache 
         *  
         * The entry with the specified key value will be removed from cache and it's usage count information will be erased. Size of the cache will be decreased. 
         *  For non-existing entries nothing will be done.
         *  
         * \param <x> Key to remove.  
         *  
         * \return 1 when entry is removed (ie number of removed emtries, which is always 1, as keys are unique) or zero when nothing was done. 
         */
        size_type erase ( const key_type& x ) throw() {
            size_type ret=_storage.erase(x);
            _policy->remove(x);

            _currEntries--;

            return ret;
        }

        /*!
         * \brief Insert element to the cache 
         *  
         * The cache is extended by inserting a single new element. This effectively increases the cache size. Because cache do not allow for duplicate key values, the insertion operation checks for each element inserted whether another element exists already in the container with the same key value, if so, the element is not inserted and its mapped value is not changed in any way. 
         * Extension of cache could result in removal of some elements, depending of the cache fullness and used policy. It is also possible, that removal of excessive entries 
         * will fail, therefore insert operation will fail too. 
         * 
         * \throw <exception_cache_full>  Thrown when there are no available space in the cache and policy doesn't allows removal of elements. 
         * \throw <exception_invalid_key> Thrown when the policy doesn't accepts the key 
         *  
         * \return true if the new elemented was inserted or false if an element with the same key existed. 
         */
        bool insert(Key _k, Data _d) throw(exception_cache_full,exception_invalid_key) {
            while (this->_currEntries >= this->_maxEntries) {
                _victim<Key> victim=_policy->victim();
                if (!victim) {
                    throw exception_cache_full("The cache is full and no element can be expired at the moment. Remove some elements manually");
                }
                this->erase(*victim);
            }

            _policy->insert(_k);


            bool result=_storage.insert(value_type(_k,_d)).second;
            if (result) {
                _currEntries++;
            }

            return result;
        }

        /*!
         * \brief Maximum cache size accessor
         *  
         * Returns the maximum number of elements that the cache object can hold. This is the maximum potential size the cache can reach. It is specified at 
         * construction time and cannot be modified (with excaption of \link cache::swap swap operation \endlink) 
         *  
         * \return The maximum number of elements a cache can have as its content. 
         *  
         * \see size 
         */
        size_type max_size() const throw() {
            return this->_maxEntries;
        }

        /*! \brief Counts entries in the cache.
          *  
          *  Provides information on number of cache entries. For checking, whether the cache empty or not, please use the \link cache::empty empty function \endlink 
          *  
          *  \return Number of object in the cache (size of cache)
          *  
          *  \see empty
          *  \see clear
          */        
        size_type size() const throw() {
            assert(this->_currEntries==_storage.size());
            return this->_currEntries;
        }

        /*!
         * \brief Access cache data 
         *  
         * Accessor to the data (values) stored in cache. If the specified key exists in the cache, it's usage count will be touched and reference to the element is returned. 
         * The data object itself is kept in the cache, so the reference will be valid until it is removed (either manually or due to cache overflow) or cache object destroyed. 
         *  
         * \param <_k> key to the data 
         * 
         * \throw  <exception_invalid_key> Thrown when non-existent key is supplied. You could use \link cache::check check member \endlink or \link cache::count count member \endlink to check cache existence prior to fetching the data
         *  
         * \return constand reference to the data, mapped by the key. of type Data of course. 
         *  
         * \see check 
         */
        const Data& fetch(const Key& _k) throw(exception_invalid_key) {
            if (!check(_k)) {
                throw exception_invalid_key("Key is not in cache",_k);
            }
            _policy->touch(_k);
            return (*(_storage.find(_k))).second;
        }

        /*!
         * \brief Check for the key presence in cache
         *  
         *  Tests, whether cache is exist in the cache or not. During check it's usage count is increased, as opposed to \link cache::count count member \endlink
         *  
         *  \param <_k> key to test
         *  
         * \return true if key exists in the cache and false otherwise 
         *  
         * \see count 
         */
        const bool check(const Key& _k) throw() {
            _policy->touch(_k);
            return _storage.count(_k)==1;
        }

        /*!
         * \brief Increase usage count for entry 
         *  
         *  Touches an entry in cache, simulating access to it. Usefull to promote (or devote, depending on policy) item in the cache, reducing(or increasing)
         *  risk for expiration for it.
         *  
         *  \param _k key to touch
         *  
         */
        void touch(const Key& _k) throw() {
            _policy->touch(_k);
        }
        //@}

        //@{
        /*!
         * \brief Copy cache content
         *  
         * Assigns a copy of the elements in x as the new content for the cache. Usage counts for entries are copied too.
         * The elements contained in the object before the call are dropped, and replaced by copies of those in cache x, if any. 
         * After a call to this member function, both the map object and x will have the same size and compare equal to each other.
         *  
         * \param <x> a cache object with the same template parameters 
         *  
         * \return *this 
         *  
         * \see swap 
         */
        cache_multimap<Key,Data,Policy,Compare,Allocator>& operator= ( const cache_multimap<Key,Data,Policy,Compare,Allocator>& x) throw() {
            this->_storage=x._storage;
            this->_maxEntries=x._maxEntries;
            this->_currEntries=this->_storage.size();

            policy_type localPolicy(*x._policy);
            this->_policy = policyAlloc.allocate(1);
            policyAlloc.construct(this->_policy,localPolicy);
            return *this;
        }

        /*!
         * \brief A copy constructor
         * 
         *  The object is initialized to have the same contents and properties as the x cache object.
         *  
         *  \param <x> a cache object with the same template parameters 
         */
        cache_multimap(const cache_multimap<Key,Data,Policy,Compare,Allocator>& x) throw() {
            *this=x;
        }
        /*!
         * \brief Primary constructor. 
         *  
         * Constructs an empty cache object and sets a maximum size for it. It is the only way to set size for a cache and it can't be changed later. 
         * You could also  pass optional comparator object, compatible with Compare. 
         *  
         * \param <size> Maximum number of entries, allowed in the cache. 
         * \param <comp> Comparator object, compatible with Compare type. Defaults to Compare() 
         * 
         */
        explicit cache_multimap(const size_type size, const Compare& comp = Compare()) throw() {
            this->_storage=storageType(comp, Allocator<std::pair<const Key, Data> >());
            this->_maxEntries=size;
            this->_currEntries=0;

            policy_type localPolicy(size);
            this->_policy = policyAlloc.allocate(1);
            policyAlloc.construct(this->_policy,localPolicy);
        }

        /*!
         * \brief destructor 
         *  
         * Destructs the cache object. This calls each of the cache element's destructors, and deallocates all the storage capacity allocated by the cache. 
         * 
         */
        ~cache_multimap() {
            policyAlloc.destroy(this->_policy);
            policyAlloc.deallocate(this->_policy,1);
        }
        //@}
    };
}

#endif /* STLCACHE_STLCACHE_MULTIMAP_HPP_INCLUDED */

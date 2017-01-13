//
// Copyright (C) 2016 Sebastien Besombes
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_CONTAINER_HPP_INCLUDED
#define STLCACHE_CONTAINER_HPP_INCLUDED

namespace stlcache {
    /*! \brief Abstract interface cache container.
     * 
     * Container class defines the internal container for caching.
     * 
     *     \tparam <Key> The cache's Key data type
     *     \tparam <Data> Type of data to be stored by the container.
     * 
     *     \see stlcache::cache
     *     \see stlcache::container_map
     *     \see stlcache::container_multimap
     *     \see stlcache::container_unordered_map
     *     \see stlcache::container_unordered_multimap
     * 
     * \author chollya (5/19/2016)
     */
    template <class Key, class Data>
    struct container {
    };

}

#endif /* STLCACHE_CONTAINER_HPP_INCLUDED */

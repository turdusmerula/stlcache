//
// Copyright (C) 2016 Sebastien Besombes
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_CONTAINER_MAP_HPP_INCLUDED
#define STLCACHE_CONTAINER_MAP_HPP_INCLUDED

#include <map>

namespace stlcache {

	template <class Key, class Data, class Compare=std::less<Key>>
    struct container_multimap {
		using allocator = std::allocator<std::pair<const Key, Data>> ;
		using storageType = std::map<Key, Data, Compare, allocator> ;
    };

}

#endif /* STLCACHE_CONTAINER_MAP_HPP_INCLUDED */

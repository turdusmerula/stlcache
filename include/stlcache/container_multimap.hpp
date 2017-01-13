//
// Copyright (C) 2016 Sebastien Besombes
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_CONTAINER_MULTIMAP_HPP_INCLUDED
#define STLCACHE_CONTAINER_MULTIMAP_HPP_INCLUDED

#include <map>

namespace stlcache {

	template <class Key, class Data, class Compare=std::less<Key>, class Allocator=std::allocator<std::pair<const Key, Data>>>
    struct container_multimap {
		using storageType = std::multimap<Key, Data, Compare, allocator> ;
    };

}

#endif /* STLCACHE_CONTAINER_MULTIMAP_HPP_INCLUDED */

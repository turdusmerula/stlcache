//
// Copyright (C) 2016 Sebastien Besombes
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STLCACHE_CONTAINER_MAP_HPP_INCLUDED
#define STLCACHE_CONTAINER_MAP_HPP_INCLUDED

namespace stlcache {

	template <class Key, class Data>
	struct _container_map_type {
		using compare_type = std::less<Key> ;

		template <class T>
		using allocator_type = std::allocator<T> ;

		using map_type = std::map<Key, Data, compare_type, allocator_type<std::pair<const Key, Data>> > ;
	};

    struct container_map {
		template <class Key, class Data>
		struct bind : _container_map_type<Key,Data> {
		} ;
    };

}

#endif /* STLCACHE_CONTAINER_MAP_HPP_INCLUDED */

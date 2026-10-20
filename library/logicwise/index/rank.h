// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "valid_extent.h"
#include <cstddef> //用于 std::size_t
#include <cassert> //用于 assert


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	template<typename Arrangement>
	constexpr std::size_t rank(typename Arrangement::extent_type extent, typename Arrangement::index_type index)
	{
		if constexpr (requires{ Arrangement::rank(extent, index); })
		{
			return Arrangement::rank(extent, index);
		}
		else
		{
			typename Arrangement::forward_index_traverser index_traverser{ extent };
			std::size_t scalar_index{ 0 };

			while (!index_traverser.done())
			{
				if (index_traverser.state() == index)
				{
					return scalar_index;
				}

				index_traverser.step();
				++scalar_index;
			}

			assert(false && "[logicwise] Index out of range.");

			return static_cast<std::size_t>(-1);
		}
	}

	template<typename Arrangement, auto Extent, typename Arrangement::index_type Index>
		requires ValidExtent<Arrangement, Extent>
	constexpr std::size_t rank()
	{
		return rank<Arrangement>(Extent, Index);
	}

	template<typename Arrangement>
	constexpr typename Arrangement::index_type unrank(typename Arrangement::extent_type extent, std::size_t scalar_index)
	{
		if constexpr (requires{ Arrangement::unrank(extent, scalar_index); })
		{
			return Arrangement::unrank(extent, scalar_index);
		}
		else
		{
			typename Arrangement::forward_index_traverser index_traverser{ extent };

			for (std::size_t i{ 0 }; i < scalar_index; ++i)
			{
				index_traverser.step();
			}

			return index_traverser.state();
		}
	}

	template<typename Arrangement, auto Extent, std::size_t ScalarIndex>
		requires ValidExtent<Arrangement, Extent>
	constexpr typename Arrangement::index_type unrank()
	{
		return unrank<Arrangement>(Extent, ScalarIndex);
	}

}

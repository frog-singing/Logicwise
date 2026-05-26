// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/detail/safe_integer_cast.h>
#include <cstddef> //用于 std::size_t
#include <algorithm> //用于 std::max
#include <cassert> //用于 assert


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::二部 arrangement::bipartite================================================================================

	//填充对齐对
	struct zip_pair_padding : bipartite_arrangement_tag, arrangement_with_padding_tag
	{
		using extent_type = Extent2D;
		using index_trait = IndexTraitPadding2D;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

		static constexpr std::size_t index_count(extent_type extent)
		{
			return (std::max)(extent.i(), extent.j());
		}

		struct forward_index_traverser
		{
			const index_integer_type extent_i, extent_j, max_extent;
			index_integer_type linear_index{ 0 };

			forward_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr forward_index_traverser(extent_type extent) noexcept
				: extent_i{ safe_integer_cast<index_integer_type>(extent.i()) }
				, extent_j{ safe_integer_cast<index_integer_type>(extent.j()) }
				, max_extent{ (std::max)(extent_i, extent_j) }
			{}

			constexpr bool done() const noexcept { return linear_index >= max_extent; }

			constexpr auto state() const noexcept
			{
				assert(!done() && linear_index >= 0 && "[logicwise] Accessing illegal index.");

				return index_type
				{
					{
						static_cast<std::size_t>(linear_index),
						static_cast<std::size_t>(linear_index)
					},
					{
						linear_index >= extent_i,
						linear_index >= extent_j
					}
				};
			}

			constexpr void step() noexcept { ++linear_index; }
		};

		struct reverse_index_traverser
		{
			const index_integer_type extent_i, extent_j;
			index_integer_type linear_index;

			reverse_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr reverse_index_traverser(extent_type extent) noexcept
				: extent_i{ safe_integer_cast<index_integer_type>(extent.i()) }
				, extent_j{ safe_integer_cast<index_integer_type>(extent.j()) }
				, linear_index{ (std::max)(extent_i, extent_j) - 1 }
			{}

			constexpr bool done() const noexcept { return linear_index < 0; }

			constexpr auto state() const noexcept
			{
				assert(!done() && "[logicwise] Accessing illegal index.");

				return index_type
				{
					{
						static_cast<std::size_t>(linear_index),
						static_cast<std::size_t>(linear_index)
					},
					{
						linear_index >= extent_i,
						linear_index >= extent_j
					}
				};
			}

			constexpr void step() noexcept { --linear_index; }
		};

		using fast_index_traverser = reverse_index_traverser;
		using light_index_traverser = reverse_index_traverser;

	};

}

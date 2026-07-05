// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/detail/safe_integer_cast.h>
#include <cstddef> //用于 std::size_t
#include <cassert> //用于 assert


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::逐对 arrangement::pairwise================================================================================

	//保序组合对
	struct combination_pair : pairwise_arrangement_tag
	{
		using extent_type = Extent1D;
		using index_trait = IndexTrait2D;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

		static constexpr std::size_t index_count(extent_type extent) noexcept
		{
			std::size_t extent_i{ extent.i() };
			return extent_i < 2 ? 0 : extent_i * (extent_i - 1) / 2;
		}

		struct forward_index_traverser
		{
			const index_integer_type extent_i;
			index_integer_type index_i{ 0 }, index_j{ 1 };

			forward_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr forward_index_traverser(extent_type extent) noexcept
				: extent_i{ safe_integer_cast<index_integer_type>(extent.i()) }
			{}

			constexpr bool done() const noexcept { return index_j >= extent_i; }

			constexpr auto state() const noexcept
			{
				assert(!done() && index_j >= 0 && "[logicwise] Accessing illegal index.");

				return index_type
				{
					static_cast<std::size_t>(index_i),
					static_cast<std::size_t>(index_j)
				};
			}

			constexpr void step() noexcept
			{
				++index_j;

				//不要用 index_j >= extent_i，否则处于非法索引时连续 step 会导致无效开销
				if (index_j == extent_i)
				{
					++index_i;
					index_j = index_i + 1;
				}
			}
		};

		struct reverse_index_traverser
		{
			const index_integer_type max_index;
			index_integer_type index_j, index_i;

			reverse_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr reverse_index_traverser(extent_type extent) noexcept
				: max_index{ safe_integer_cast<index_integer_type>(extent.i()) - 1 }
				, index_j{ max_index }
				, index_i{ index_j - 1 }
			{}

			constexpr bool done() const noexcept { return index_i < 0; }

			constexpr auto state() const noexcept
			{
				assert(!done() && "[logicwise] Accessing illegal index.");

				return index_type
				{
					static_cast<std::size_t>(index_i),
					static_cast<std::size_t>(index_j)
				};
			}

			constexpr void step() noexcept
			{
				--index_j;

				if (index_j <= index_i)
				{
					--index_i;
					index_j = max_index;
				}
			}
		};

		using fast_index_traverser = reverse_index_traverser;
		using light_index_traverser = forward_index_traverser;

	};

}

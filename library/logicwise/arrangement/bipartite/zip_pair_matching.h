// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/detail/extent_component_mismatch.h>
#include <logicwise/detail/safe_integer_cast.h>
#include <cstddef> //用于 std::size_t
#include <algorithm> //用于 std::min
#include <cassert> //用于 assert


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::二部 arrangement::bipartite================================================================================

	//匹配对齐对
	struct zip_pair_matching : bipartite_arrangement_tag
	{
		using extent_type = Extent2D;
		using index_trait = IndexTrait2D;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

		template<extent_type Extent>
		static constexpr bool ValidExtent = []{
			static_assert(Extent.i() == Extent.j(), "[logicwise] Extent component mismatch.");
			return true;
		}();

		static constexpr void verify_extent(extent_type extent) noexcept
		{
			if (extent.i() != extent.j()) [[unlikely]] { extent_component_mismatch(); }
		}

		static constexpr std::size_t index_count(extent_type extent) noexcept
		{
			verify_extent(extent);
			return extent.i();
		}

		struct forward_index_traverser
		{
			const index_integer_type uniform_extent;
			index_integer_type uniform_index{ 0 };

			forward_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr forward_index_traverser(extent_type extent) noexcept
				: uniform_extent{ safe_integer_cast<index_integer_type>(extent.i()) }
			{
				verify_extent(extent);
			}

			constexpr bool done() const noexcept { return uniform_index >= uniform_extent; }

			constexpr auto state() const noexcept
			{
				assert(!done() && uniform_index >= 0 && "[logicwise] Accessing illegal index.");

				return index_type
				{
					static_cast<std::size_t>(uniform_index),
					static_cast<std::size_t>(uniform_index)
				};
			}

			constexpr void step() noexcept { ++uniform_index; }
		};

		struct reverse_index_traverser
		{
			index_integer_type uniform_index;

			reverse_index_traverser() = delete; //禁用默认构造函数

			explicit constexpr reverse_index_traverser(extent_type extent) noexcept
				: uniform_index{ safe_integer_cast<index_integer_type>(extent.i()) - 1 }
			{
				verify_extent(extent);
			}

			constexpr bool done() const noexcept { return uniform_index < 0; }

			constexpr auto state() const noexcept
			{
				assert(!done() && "[logicwise] Accessing illegal index.");

				return index_type
				{
					static_cast<std::size_t>(uniform_index),
					static_cast<std::size_t>(uniform_index)
				};
			}

			constexpr void step() noexcept { --uniform_index; }
		};

		using fast_index_traverser = reverse_index_traverser;
		using light_index_traverser = reverse_index_traverser;

	};

}

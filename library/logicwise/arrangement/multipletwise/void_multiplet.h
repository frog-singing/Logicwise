// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::逐多元组 arrangement::multipletwise================================================================================

	struct void_multiplet : multipletwise_arrangement_tag
	{
		using extent_type = Extent1D;
		using index_trait = IndexTrait<0>;
		using index_type = typename index_trait::index_type;

		static constexpr std::size_t index_count(extent_type)
		{
			return 0;
		}

		struct default_index_traverser
		{
			explicit constexpr default_index_traverser(extent_type) noexcept {}
			constexpr bool done() const noexcept { return true; }
			constexpr auto state() const noexcept { return index_type{}; }
			constexpr void step() noexcept {}
		};

		using forward_index_traverser = default_index_traverser;
		using reverse_index_traverser = default_index_traverser;
		using fast_index_traverser = default_index_traverser;
		using light_index_traverser = default_index_traverser;

	};

}

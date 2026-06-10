// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>


namespace logicwise::detail
{
	template<typename ExtentType>
	struct base_void;

	struct multipletwise_void;
	struct multipartite_void;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::虚空维度 arrangement::voidwise================================================================================

	//基础虚空
	template<typename ExtentType>
	struct base_void
	{
		using extent_type = ExtentType;
		using index_trait = IndexTrait0D;
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

	//逐多元组虚空
	struct multipletwise_void : base_void<Extent1D>, multipletwise_arrangement_tag {};

	//多部虚空
	struct multipartite_void : base_void<Extent0D>, multipartite_arrangement_tag {};
}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/detail/safe_integer_cast.h>
#include <logicwise/arrangement/voidwise/void.h>
#include <logicwise/arrangement/elementwise/element.h>
#include <logicwise/arrangement/pairwise/circular_adjacent_pair.h>
#include <cstddef> //用于 std::size_t
#include <cassert> //用于 assert
#include <concepts> //用于 std::integral，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::逐多元组 arrangement::multipletwise================================================================================

	//全排列多元组
	template<std::size_t Arity>
	struct permutation_multiplet_impl : multipletwise_arrangement_tag
	{
		using type = permutation_multiplet_impl;

		using extent_type = Extent1D;
		using index_trait = IndexTrait<Arity>;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

	};

	template<> struct permutation_multiplet_impl<0> { using type = multipletwise_void; };
	template<> struct permutation_multiplet_impl<1> { using type = multipletwise_element; };
	template<> struct permutation_multiplet_impl<2> { using type = permutation_pair; };

	template<std::integral auto Arity> requires (Arity >= 0)
	using permutation_multiplet = typename permutation_multiplet_impl<static_cast<std::size_t>(Arity)>::type;

}

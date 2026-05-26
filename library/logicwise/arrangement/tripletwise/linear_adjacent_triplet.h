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
	//排布::逐三元组 arrangement::tripletwise================================================================================

	//单向线性相邻三元组
	struct linear_adjacent_triplet : multipartite_arrangement_tag
	{
		using extent_type = Extent1D;
		using index_trait = IndexTrait3D;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

	};

}

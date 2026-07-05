// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "arrangement/elementwise.h"
#include "arrangement/pairwise.h"
#include "arrangement/tripletwise.h"
#include "arrangement/multipletwise.h"
#include "arrangement/bipartite.h"
#include "arrangement/multipartite.h"


//逻辑维度::排布
namespace logicwise::arrangement
{
	//排布::逐元素 arrangement::elementwise================================================================================

	using detail::element; //单个元素

	//排布::逐对 arrangement::pairwise================================================================================

	using detail::permutation_pair; //全排列对
	using detail::combination_pair; //保序组合对
	using detail::linear_adjacent_pair; //单向线性相邻对
	using detail::circular_adjacent_pair; //单向环状相邻对

	//排布::逐三元组 arrangement::tripletwise================================================================================

	//[未实现] [unimplemented]

	using detail::permutation_triplet; //全排列三元组
	using detail::combination_triplet; //保序组合三元组
	using detail::linear_adjacent_triplet; //单向线性相邻三元组
	using detail::circular_adjacent_triplet; //单向环状相邻三元组

	//排布::二部 arrangement::bipartite================================================================================

	using detail::cartesian_pair; //笛卡尔积对
	using detail::zip_pair_truncation; //截断对齐对
	using detail::zip_pair_matching; //匹配对齐对
	using detail::zip_pair_padding; //填充对齐对

	//排布::多部 arrangement::multipartite================================================================================

	//[未实现] [unimplemented]

	using detail::cartesian_tuple; //笛卡尔积多元组
	using detail::zip_tuple_truncation; //截断对齐多元组
	using detail::zip_tuple_matching; //匹配对齐多元组
	using detail::zip_tuple_padding; //填充对齐多元组

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "traversal/elementwise.h"
#include "traversal/pairwise.h"
#include "traversal/tripletwise.h"
#include "traversal/bipartite.h"
#include "traversal/bipartite_padding.h"
#include "traversal/multipartite.h"
#include "traversal/multipartite_padding.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename ModeOption, typename Arrangement>
	struct rangewise_traversal_impl;

	template<typename ModeOption, ElementwiseArrangement Arrangement>
	struct rangewise_traversal_impl<ModeOption, Arrangement>
	{
		using type = elementwise_traversal<ModeOption, Arrangement>;
	};

	template<typename ModeOption, PairwiseArrangement Arrangement>
	struct rangewise_traversal_impl<ModeOption, Arrangement>
	{
		using type = pairwise_traversal<ModeOption, Arrangement>;
	};

	template<typename ModeOption, TripletwiseArrangement Arrangement>
	struct rangewise_traversal_impl<ModeOption, Arrangement>
	{
		using type = tripletwise_traversal<ModeOption, Arrangement>;
	};

	template<typename ModeOption, BipartiteArrangement Arrangement>
	struct rangewise_traversal_impl<ModeOption, Arrangement>
	{
		using type = bipartite_traversal<ModeOption, Arrangement>;
	};

	template<typename ModeOption, MultipartiteArrangement Arrangement>
	struct rangewise_traversal_impl<ModeOption, Arrangement>
	{
		using type = multipartite_traversal<ModeOption, Arrangement>;
	};


	template<typename ModeOption, typename Arrangement>
	using rangewise_traversal = typename rangewise_traversal_impl<ModeOption, Arrangement>::type;

}

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
	//行为模式::遍历 mode::traversal================================================================================

	template<typename Mode, typename Arrangement>
	struct rangewise_traversal_impl;

	template<typename Mode, ElementwiseArrangement Arrangement>
	struct rangewise_traversal_impl<Mode, Arrangement>
	{
		using type = elementwise_traversal<Mode, Arrangement>;
	};

	template<typename Mode, PairwiseArrangement Arrangement>
	struct rangewise_traversal_impl<Mode, Arrangement>
	{
		using type = pairwise_traversal<Mode, Arrangement>;
	};

	template<typename Mode, TripletwiseArrangement Arrangement>
	struct rangewise_traversal_impl<Mode, Arrangement>
	{
		using type = tripletwise_traversal<Mode, Arrangement>;
	};

	template<typename Mode, BipartiteArrangement Arrangement>
	struct rangewise_traversal_impl<Mode, Arrangement>
	{
		using type = bipartite_traversal<Mode, Arrangement>;
	};

	template<typename Mode, MultipartiteArrangement Arrangement>
	struct rangewise_traversal_impl<Mode, Arrangement>
	{
		using type = multipartite_traversal<Mode, Arrangement>;
	};


	template<typename Mode, typename Arrangement>
	using rangewise_traversal = typename rangewise_traversal_impl<Mode, Arrangement>::type;

}

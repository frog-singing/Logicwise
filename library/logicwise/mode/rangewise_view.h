// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "view/elementwise.h"
#include "view/pairwise.h"
#include "view/tripletwise.h"
#include "view/bipartite.h"
#include "view/multipartite.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename Arrangement>
	struct rangewise_view_impl;

	template<ElementwiseArrangement Arrangement>
	struct rangewise_view_impl<Arrangement>
	{
		using type = elementwise_view<Arrangement>;
	};

	template<PairwiseArrangement Arrangement>
	struct rangewise_view_impl<Arrangement>
	{
		using type = pairwise_view<Arrangement>;
	};

	template<TripletwiseArrangement Arrangement>
	struct rangewise_view_impl<Arrangement>
	{
		using type = tripletwise_view<Arrangement>;
	};

	template<BipartiteArrangement Arrangement>
	struct rangewise_view_impl<Arrangement>
	{
		using type = bipartite_view<Arrangement>;
	};

	template<MultipartiteArrangement Arrangement>
	struct rangewise_view_impl<Arrangement>
	{
		using type = multipartite_view<Arrangement>;
	};


	template<typename Arrangement>
	using rangewise_view = typename rangewise_view_impl<Arrangement>::type;

}

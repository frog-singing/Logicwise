// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "query/elementwise.h"
#include "query/pairwise.h"
#include "query/tripletwise.h"
#include "query/bipartite.h"
#include "query/multipartite.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::查询 mode::query================================================================================

	template<typename Arrangement>
	struct rangewise_query_impl;

	template<ElementwiseArrangement Arrangement>
	struct rangewise_query_impl<Arrangement>
	{
		using type = elementwise_query<Arrangement>;
	};

	template<PairwiseArrangement Arrangement>
	struct rangewise_query_impl<Arrangement>
	{
		using type = pairwise_query<Arrangement>;
	};

	template<TripletwiseArrangement Arrangement>
	struct rangewise_query_impl<Arrangement>
	{
		using type = tripletwise_query<Arrangement>;
	};

	template<BipartiteArrangement Arrangement>
	struct rangewise_query_impl<Arrangement>
	{
		using type = bipartite_query<Arrangement>;
	};

	template<MultipartiteArrangement Arrangement>
	struct rangewise_query_impl<Arrangement>
	{
		using type = multipartite_query<Arrangement>;
	};


	template<typename Arrangement>
	using rangewise_query = typename rangewise_query_impl<Arrangement>::type;

}

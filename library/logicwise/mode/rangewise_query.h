// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "query/elementwise.h"
#include "query/pairwise.h"
#include "query/tripletwise.h"
#include "query/bipartite.h"
#include "query/bipartite_padding.h"
#include "query/multipartite.h"
#include "query/multipartite_padding.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::查询 mode::query================================================================================

	template<typename Mode, typename Arrangement>
	struct rangewise_query_impl;

	template<typename Mode, ElementwiseArrangement Arrangement>
	struct rangewise_query_impl<Mode, Arrangement>
	{
		using type = elementwise_query<Mode, Arrangement>;
	};

	template<typename Mode, PairwiseArrangement Arrangement>
	struct rangewise_query_impl<Mode, Arrangement>
	{
		using type = pairwise_query<Mode, Arrangement>;
	};

	template<typename Mode, TripletwiseArrangement Arrangement>
	struct rangewise_query_impl<Mode, Arrangement>
	{
		using type = tripletwise_query<Mode, Arrangement>;
	};

	template<typename Mode, BipartiteArrangement Arrangement>
	struct rangewise_query_impl<Mode, Arrangement>
	{
		using type = bipartite_query<Mode, Arrangement>;
	};

	template<typename Mode, MultipartiteArrangement Arrangement>
	struct rangewise_query_impl<Mode, Arrangement>
	{
		using type = multipartite_query<Mode, Arrangement>;
	};


	template<typename Mode, typename Arrangement>
	using rangewise_query = typename rangewise_query_impl<Mode, Arrangement>::type;

}

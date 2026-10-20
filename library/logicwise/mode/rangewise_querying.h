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
	//模式::查询 mode::query================================================================================

	template<typename ModeOption, typename Arrangement>
	struct rangewise_querying_impl;

	template<typename ModeOption, ElementwiseArrangement Arrangement>
	struct rangewise_querying_impl<ModeOption, Arrangement>
	{
		using type = elementwise_querying<ModeOption, Arrangement>;
	};

	template<typename ModeOption, PairwiseArrangement Arrangement>
	struct rangewise_querying_impl<ModeOption, Arrangement>
	{
		using type = pairwise_querying<ModeOption, Arrangement>;
	};

	template<typename ModeOption, TripletwiseArrangement Arrangement>
	struct rangewise_querying_impl<ModeOption, Arrangement>
	{
		using type = tripletwise_querying<ModeOption, Arrangement>;
	};

	template<typename ModeOption, BipartiteArrangement Arrangement>
	struct rangewise_querying_impl<ModeOption, Arrangement>
	{
		using type = bipartite_querying<ModeOption, Arrangement>;
	};

	template<typename ModeOption, MultipartiteArrangement Arrangement>
	struct rangewise_querying_impl<ModeOption, Arrangement>
	{
		using type = multipartite_querying<ModeOption, Arrangement>;
	};


	template<typename ModeOption, typename Arrangement>
	using rangewise_querying = typename rangewise_querying_impl<ModeOption, Arrangement>::type;

}

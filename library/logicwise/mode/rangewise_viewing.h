// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "view/elementwise.h"
#include "view/pairwise.h"
#include "view/tripletwise.h"
#include "view/bipartite.h"
#include "view/bipartite_padding.h"
#include "view/multipartite.h"
#include "view/multipartite_padding.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	template<typename ModeOption, typename Arrangement>
	struct rangewise_viewing_impl;

	template<typename ModeOption, ElementwiseArrangement Arrangement>
	struct rangewise_viewing_impl<ModeOption, Arrangement>
	{
		using type = elementwise_viewing<ModeOption, Arrangement>;
	};

	template<typename ModeOption, PairwiseArrangement Arrangement>
	struct rangewise_viewing_impl<ModeOption, Arrangement>
	{
		using type = pairwise_viewing<ModeOption, Arrangement>;
	};

	template<typename ModeOption, TripletwiseArrangement Arrangement>
	struct rangewise_viewing_impl<ModeOption, Arrangement>
	{
		using type = tripletwise_viewing<ModeOption, Arrangement>;
	};

	template<typename ModeOption, BipartiteArrangement Arrangement>
	struct rangewise_viewing_impl<ModeOption, Arrangement>
	{
		using type = bipartite_viewing<ModeOption, Arrangement>;
	};

	template<typename ModeOption, MultipartiteArrangement Arrangement>
	struct rangewise_viewing_impl<ModeOption, Arrangement>
	{
		using type = multipartite_viewing<ModeOption, Arrangement>;
	};


	template<typename ModeOption, typename Arrangement>
	using rangewise_viewing = typename rangewise_viewing_impl<ModeOption, Arrangement>::type;

}

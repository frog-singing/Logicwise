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
	//行为模式::视图化 mode::view================================================================================

	template<typename Mode, typename Arrangement>
	struct rangewise_viewing_impl;

	template<typename Mode, ElementwiseArrangement Arrangement>
	struct rangewise_viewing_impl<Mode, Arrangement>
	{
		using type = elementwise_viewing<Mode, Arrangement>;
	};

	template<typename Mode, PairwiseArrangement Arrangement>
	struct rangewise_viewing_impl<Mode, Arrangement>
	{
		using type = pairwise_viewing<Mode, Arrangement>;
	};

	template<typename Mode, TripletwiseArrangement Arrangement>
	struct rangewise_viewing_impl<Mode, Arrangement>
	{
		using type = tripletwise_viewing<Mode, Arrangement>;
	};

	template<typename Mode, BipartiteArrangement Arrangement>
	struct rangewise_viewing_impl<Mode, Arrangement>
	{
		using type = bipartite_viewing<Mode, Arrangement>;
	};

	template<typename Mode, MultipartiteArrangement Arrangement>
	struct rangewise_viewing_impl<Mode, Arrangement>
	{
		using type = multipartite_viewing<Mode, Arrangement>;
	};


	template<typename Mode, typename Arrangement>
	using rangewise_viewing = typename rangewise_viewing_impl<Mode, Arrangement>::type;

}

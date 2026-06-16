// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "verification/elementwise.h"
#include "verification/pairwise.h"
#include "verification/tripletwise.h"
#include "verification/bipartite.h"
#include "verification/bipartite_padding.h"
#include "verification/multipartite.h"
#include "verification/multipartite_padding.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::verification================================================================================

	template<typename Quantifier, typename Arrangement>
	struct rangewise_verification_impl;

	template<typename Quantifier, ElementwiseArrangement Arrangement>
	struct rangewise_verification_impl<Quantifier, Arrangement>
	{
		using type = elementwise_verification<Quantifier, Arrangement>;
	};

	template<typename Quantifier, PairwiseArrangement Arrangement>
	struct rangewise_verification_impl<Quantifier, Arrangement>
	{
		using type = pairwise_verification<Quantifier, Arrangement>;
	};

	template<typename Quantifier, TripletwiseArrangement Arrangement>
	struct rangewise_verification_impl<Quantifier, Arrangement>
	{
		using type = tripletwise_verification<Quantifier, Arrangement>;
	};

	template<typename Quantifier, BipartiteArrangement Arrangement>
	struct rangewise_verification_impl<Quantifier, Arrangement>
	{
		using type = bipartite_verification<Quantifier, Arrangement>;
	};

	template<typename Quantifier, MultipartiteArrangement Arrangement>
	struct rangewise_verification_impl<Quantifier, Arrangement>
	{
		using type = multipartite_verification<Quantifier, Arrangement>;
	};


	template<typename Quantifier, typename Arrangement>
	using rangewise_verification = typename rangewise_verification_impl<Quantifier, Arrangement>::type;

}

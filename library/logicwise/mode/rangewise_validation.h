// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include "validation/elementwise.h"
#include "validation/pairwise.h"
#include "validation/tripletwise.h"
#include "validation/bipartite.h"
#include "validation/bipartite_padding.h"
#include "validation/multipartite.h"
#include "validation/multipartite_padding.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

	template<typename Quantifier, typename Arrangement>
	struct rangewise_validation_impl;

	template<typename Quantifier, ElementwiseArrangement Arrangement>
	struct rangewise_validation_impl<Quantifier, Arrangement>
	{
		using type = elementwise_validation<Quantifier, Arrangement>;
	};

	template<typename Quantifier, PairwiseArrangement Arrangement>
	struct rangewise_validation_impl<Quantifier, Arrangement>
	{
		using type = pairwise_validation<Quantifier, Arrangement>;
	};

	template<typename Quantifier, TripletwiseArrangement Arrangement>
	struct rangewise_validation_impl<Quantifier, Arrangement>
	{
		using type = tripletwise_validation<Quantifier, Arrangement>;
	};

	template<typename Quantifier, BipartiteArrangement Arrangement>
	struct rangewise_validation_impl<Quantifier, Arrangement>
	{
		using type = bipartite_validation<Quantifier, Arrangement>;
	};

	template<typename Quantifier, MultipartiteArrangement Arrangement>
	struct rangewise_validation_impl<Quantifier, Arrangement>
	{
		using type = multipartite_validation<Quantifier, Arrangement>;
	};


	template<typename Quantifier, typename Arrangement>
	using rangewise_validation = typename rangewise_validation_impl<Quantifier, Arrangement>::type;

}

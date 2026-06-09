// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::derived_from，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//排布 arrangement================================================================================

	//排布标签
	struct arrangement_tag {};
	struct elementwise_arrangement_tag		: arrangement_tag {};
	struct pairwise_arrangement_tag			: arrangement_tag {};
	struct tripletwise_arrangement_tag		: arrangement_tag {};
	struct multipletwise_arrangement_tag	: arrangement_tag {};
	struct bipartite_arrangement_tag		: arrangement_tag {};
	struct multipartite_arrangement_tag		: arrangement_tag {};

	//排布约束
	template<typename TargetArrangement>
	concept Arrangement = std::derived_from<TargetArrangement, arrangement_tag>;

	template<typename Arrangement>
	concept ElementwiseArrangement		= std::derived_from<Arrangement, elementwise_arrangement_tag>;

	template<typename Arrangement>
	concept PairwiseArrangement			= std::derived_from<Arrangement, pairwise_arrangement_tag>;

	template<typename Arrangement>
	concept TripletwiseArrangement		= std::derived_from<Arrangement, tripletwise_arrangement_tag>;

	template<typename Arrangement>
	concept MultipletwiseArrangement	= std::derived_from<Arrangement, multipletwise_arrangement_tag>;

	template<typename Arrangement>
	concept BipartiteArrangement		= std::derived_from<Arrangement, bipartite_arrangement_tag>;

	template<typename Arrangement>
	concept MultipartiteArrangement		= std::derived_from<Arrangement, multipartite_arrangement_tag>;

	//带填充排布标签
	struct arrangement_with_padding_tag {};

	//带填充排布约束，用于在具体排布分支中特化
	template<typename ValidArrangement>
	concept ArrangementWithPadding = std::derived_from<ValidArrangement, arrangement_with_padding_tag>;

}

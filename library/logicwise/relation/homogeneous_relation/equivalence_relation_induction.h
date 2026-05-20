// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//同质关系::等价 homogeneous relation::equivalence================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//等价关系诱导
	template<auto EquivalenceRelation>
	struct equivalence_relation_induction
	{
		// ~ 等价关系
		//equivalence_relation

		// ≈ 相容关系
		static constexpr auto tolerance_relation{ EquivalenceRelation };
	};

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//等价关系诱导
	template<auto EquivalenceRelation>
	struct equivalence_relation_induction
	{
		// ~ 等价关系
		//equivalence_relation

		// ≈ 相容关系
		static constexpr auto tolerance_relation{ EquivalenceRelation };
	};

}

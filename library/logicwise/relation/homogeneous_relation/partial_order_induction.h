// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "equivalence_relation_induction.h"
#include "property.h"


//同质关系::偏序 homogeneous relation::partial order================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//偏序关系诱导
	template<auto PartialOrder>
	struct partial_order_induction : equivalence_relation_induction<symmetric_kernel<PartialOrder>>
	{
		// ≤ 偏序关系
		//partial_order

		// ≥ 对偶偏序关系
		static constexpr auto dual_partial_order = [] <typename T1, typename T2>
		{
			return bool{ PartialOrder.template operator() < T2, T1 > () };
		};

		// ~ 等价关系
		//PartialOrder<T1, T2>() && PartialOrder<T2, T1>()
		static constexpr auto equivalence_relation{ symmetric_kernel<PartialOrder> };

		// < 严格偏序关系
		static constexpr auto strict_partial_order = [] <typename T1, typename T2>
		{
			return (PartialOrder.template operator() < T1, T2 > ()) &&
				!(PartialOrder.template operator() < T2, T1 > ());
		};

		// > 对偶严格偏序关系
		static constexpr auto dual_strict_partial_order = [] <typename T1, typename T2>
		{
			return !(PartialOrder.template operator() < T1, T2 > ()) &&
				(PartialOrder.template operator() < T2, T1 > ());
		};

		// ∥ 不可比关系
		static constexpr auto incomparability_relation = [] <typename T1, typename T2>
		{
			return !(PartialOrder.template operator() < T1, T2 > ()) &&
				!(PartialOrder.template operator() < T2, T1 > ());
		};

	};

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//偏序关系诱导
	template<auto PartialOrder>
	struct partial_order_induction : equivalence_relation_induction<symmetric_kernel<PartialOrder>>
	{
		// ≤ 偏序关系
		//partial_order

		// ≥ 对偶偏序关系
		static constexpr auto dual_partial_order = [] <auto V1, auto V2>
		{
			return PartialOrder.template operator() < V2, V1 > ();
		};

		// ~ 等价关系
		//PartialOrder<T1, T2>() && PartialOrder<T2, T1>()
		static constexpr auto equivalence_relation{ symmetric_kernel<PartialOrder> };

		// < 严格偏序关系
		static constexpr auto strict_partial_order = [] <auto V1, auto V2>
		{
			return (PartialOrder.template operator() < V1, V2 > ()) &&
				!(PartialOrder.template operator() < V2, V1 > ());
		};

		// > 对偶严格偏序关系
		static constexpr auto dual_strict_partial_order = [] <auto V1, auto V2>
		{
			return !(PartialOrder.template operator() < V1, V2 > ()) &&
				(PartialOrder.template operator() < V2, V1 > ());
		};

		// ∥ 不可比关系
		static constexpr auto incomparability_relation = [] <auto V1, auto V2>
		{
			return !(PartialOrder.template operator() < V1, V2 > ()) &&
				!(PartialOrder.template operator() < V2, V1 > ());
		};

	};

}

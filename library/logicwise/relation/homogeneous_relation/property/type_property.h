// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/relation/homogeneous_relation/equivalence.h>


//同质关系::性质 homogeneous relation::property================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//自反性
	//需要检查：所有单个元素 <all_of, element>
	template<auto HomogeneousRelation>
	inline constexpr auto reflexivity = [] <typename T>
	{
		return bool{ HomogeneousRelation.template operator() < T, T > () };
	};

	//反自反性
	//需要检查：所有单个元素 <all_of, element>
	template<auto HomogeneousRelation>
	inline constexpr auto irreflexivity = [] <typename T>
	{
		return !(HomogeneousRelation.template operator() < T, T > ());
	};

	//共自反性
	//需要检查：所有全排列对 <all_of, permutation_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto coreflexivity = [] <typename T1, typename T2>
	{
		return !(HomogeneousRelation.template operator() < T1, T2 > ()) ||
			(PresetEquivalenceRelation.template operator() < T1, T2 > ());
	};

	//--------------------------------------------------------------------------------

	//对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto symmetry = [] <typename T1, typename T2>
	{
		return (HomogeneousRelation.template operator() < T1, T2 > ())
			== (HomogeneousRelation.template operator() < T2, T1 > ());
	};

	//对称核
	//在验证同质关系的反对称性时，从原关系诱导出的等价关系（需要满足自反性、对称性、传递性）
	//原同质关系不违背自反性  <==>  此关系不违背自反性
	//此关系满足对称性
	//原同质关系不违背传递性  ==>  此关系不违背传递性
	//若存在预设的等价关系，此关系不得比其更宽松
	template<auto HomogeneousRelation>
	inline constexpr auto symmetric_kernel = [] <typename T1, typename T2>
	{
		return (HomogeneousRelation.template operator() < T1, T2 > ()) &&
			(HomogeneousRelation.template operator() < T2, T1 > ());
	};

	//反对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto antisymmetry = [] <typename T1, typename T2>
	{
		return !(
			(HomogeneousRelation.template operator() < T1, T2 > ()) &&
			(HomogeneousRelation.template operator() < T2, T1 > ())
		) ||
			(PresetEquivalenceRelation.template operator() < T1, T2 > ());
	};

	//antisymmetry<HomogeneousRelation, PresetEquivalenceRelation>
	//	= coreflexivity<symmetric_kernel<HomogeneousRelation>, PresetEquivalenceRelation>

	//非对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto asymmetry = [] <typename T1, typename T2>
	{
		return !(
			(HomogeneousRelation.template operator() < T1, T2 > ()) &&
			(HomogeneousRelation.template operator() < T2, T1 > ())
		);
	};

	//--------------------------------------------------------------------------------

	//传递性
	//需要检查：所有全排列三元组 <all_of, permutation_triplet>
	template<auto HomogeneousRelation>
	inline constexpr auto transitivity = [] <typename T1, typename T2, typename T3>
	{
		return !(
			(HomogeneousRelation.template operator() < T1, T2 > ()) &&
			(HomogeneousRelation.template operator() < T2, T3 > ())
		) ||
			(HomogeneousRelation.template operator() < T1, T3 > ());
	};
	
	//欧几里得性
	//需要检查：所有全排列三元组 <all_of, permutation_triplet>
	template<auto HomogeneousRelation>
	inline constexpr auto euclidean = [] <typename T1, typename T2, typename T3>
	{
		return !(
			(HomogeneousRelation.template operator() < T1, T2 > ()) &&
			(HomogeneousRelation.template operator() < T1, T3 > ())
		) ||
			(HomogeneousRelation.template operator() < T2, T3 > ());
	};

	//连通性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto connectedness = [] <typename T1, typename T2>
	{
		return (HomogeneousRelation.template operator() < T1, T2 > ()) ||
			(HomogeneousRelation.template operator() < T2, T1 > ());
	};

	//三分性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto trichotomy = [] <typename T1, typename T2>
	{
		return (
			bool{ HomogeneousRelation.template operator() < T1, T2 > () }
			+ bool{ HomogeneousRelation.template operator() < T2, T1 > () }
			+ bool{ PresetEquivalenceRelation.template operator() < T1, T2 > () }
		) == 1;
	};

}

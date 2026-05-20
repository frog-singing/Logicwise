// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/relation/homogeneous_relation/equivalence.h>


//同质关系::性质 homogeneous relation::property================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//自反性
	//需要检查：所有单个元素 <all_of, element>
	template<auto HomogeneousRelation>
	inline constexpr auto reflexivity = [] <auto V>
	{
		return bool{ HomogeneousRelation.template operator() < V, V > () };
	};

	//反自反性
	//需要检查：所有单个元素 <all_of, element>
	template<auto HomogeneousRelation>
	inline constexpr auto irreflexivity = [] <auto V>
	{
		return !(HomogeneousRelation.template operator() < V, V > ());
	};

	//共自反性
	//需要检查：所有全排列对 <all_of, permutation_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto coreflexivity = [] <auto V1, auto V2>
	{
		return !(HomogeneousRelation.template operator() < V1, V2 > ()) ||
			(PresetEquivalenceRelation.template operator() < V1, V2 > ());
	};

	//--------------------------------------------------------------------------------

	//对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto symmetry = [] <auto V1, auto V2>
	{
		return (HomogeneousRelation.template operator() < V1, V2 > ())
			== (HomogeneousRelation.template operator() < V2, V1 > ());
	};

	//对称核
	//在验证同质关系的反对称性时，从原关系诱导出的等价关系（需要满足自反性、对称性、传递性）
	//原同质关系不违背自反性  <==>  此关系不违背自反性
	//此关系满足对称性
	//原同质关系不违背传递性  ==>  此关系不违背传递性
	//若存在预设的等价关系，此关系不得比其更宽松
	template<auto HomogeneousRelation>
	inline constexpr auto symmetric_kernel = [] <auto V1, auto V2>
	{
		return (HomogeneousRelation.template operator() < V1, V2 > ()) &&
			(HomogeneousRelation.template operator() < V2, V1 > ());
	};

	//反对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto antisymmetry = [] <auto V1, auto V2>
	{
		return !(
			(HomogeneousRelation.template operator() < V1, V2 > ()) &&
			(HomogeneousRelation.template operator() < V2, V1 > ())
		) ||
			(PresetEquivalenceRelation.template operator() < V1, V2 > ());
	};

	//antisymmetry<HomogeneousRelation, PresetEquivalenceRelation>
	//	= coreflexivity<symmetric_kernel<HomogeneousRelation>, PresetEquivalenceRelation>

	//非对称性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto asymmetry = [] <auto V1, auto V2>
	{
		return !(
			(HomogeneousRelation.template operator() < V1, V2 > ()) &&
			(HomogeneousRelation.template operator() < V2, V1 > ())
		);
	};

	//--------------------------------------------------------------------------------

	//传递性
	//需要检查：所有全排列三元组 <all_of, permutation_triplet>
	template<auto HomogeneousRelation>
	inline constexpr auto transitivity = [] <auto V1, auto V2, auto V3>
	{
		return !(
			(HomogeneousRelation.template operator() < V1, V2 > ()) &&
			(HomogeneousRelation.template operator() < V2, V3 > ())
		) ||
			(HomogeneousRelation.template operator() < V1, V3 > ());
	};
	
	//欧几里得性
	//需要检查：所有全排列三元组 <all_of, permutation_triplet>
	template<auto HomogeneousRelation>
	inline constexpr auto euclidean = [] <auto V1, auto V2, auto V3>
	{
		return !(
			(HomogeneousRelation.template operator() < V1, V2 > ()) &&
			(HomogeneousRelation.template operator() < V1, V3 > ())
		) ||
			(HomogeneousRelation.template operator() < V2, V3 > ());
	};

	//连通性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation>
	inline constexpr auto connectedness = [] <auto V1, auto V2>
	{
		return (HomogeneousRelation.template operator() < V1, V2 > ()) ||
			(HomogeneousRelation.template operator() < V2, V1 > ());
	};

	//三分性
	//需要检查：所有保序组合对 <all_of, combination_pair>
	template<auto HomogeneousRelation, auto PresetEquivalenceRelation = default_equivalence_relation>
	inline constexpr auto trichotomy = [] <auto V1, auto V2>
	{
		return (
			bool{ HomogeneousRelation.template operator() < V1, V2 > () }
			+ bool{ HomogeneousRelation.template operator() < V2, V1 > () }
			+ bool{ PresetEquivalenceRelation.template operator() < V1, V2 > () }
		) == 1;
	};

}

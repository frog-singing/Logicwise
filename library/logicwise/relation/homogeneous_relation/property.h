// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "property/type_property.h"
#include "property/value_property.h"


//同质关系::性质 homogeneous relation::property================================================================================

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::reflexivity;		//自反性		<all_of, element>
	using detail::typewise::irreflexivity;		//反自反性	<all_of, element>
	using detail::typewise::coreflexivity;		//共自反性	<all_of, permutation_pair>		PresetEquivalenceRelation

	using detail::typewise::symmetry;			//对称性		<all_of, combination_pair>
	using detail::typewise::symmetric_kernel;	//对称核
	using detail::typewise::antisymmetry;		//反对称性	<all_of, combination_pair>		PresetEquivalenceRelation
	using detail::typewise::asymmetry;			//非对称性	<all_of, combination_pair>

	using detail::typewise::transitivity;		//传递性		<all_of, permutation_triplet>
	using detail::typewise::euclidean;			//欧几里得性	<all_of, permutation_triplet>
	using detail::typewise::connectedness;		//连通性		<all_of, combination_pair>
	using detail::typewise::trichotomy;			//三分性		<all_of, combination_pair>		PresetEquivalenceRelation

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::reflexivity;		//自反性		<all_of, element>
	using detail::valuewise::irreflexivity;		//反自反性	<all_of, element>
	using detail::valuewise::coreflexivity;		//共自反性	<all_of, permutation_pair>		PresetEquivalenceRelation

	using detail::valuewise::symmetry;			//对称性		<all_of, combination_pair>
	using detail::valuewise::symmetric_kernel;	//对称核
	using detail::valuewise::antisymmetry;		//反对称性	<all_of, combination_pair>		PresetEquivalenceRelation
	using detail::valuewise::asymmetry;			//非对称性	<all_of, combination_pair>

	using detail::valuewise::transitivity;		//传递性		<all_of, permutation_triplet>
	using detail::valuewise::euclidean;			//欧几里得性	<all_of, permutation_triplet>
	using detail::valuewise::connectedness;		//连通性		<all_of, combination_pair>
	using detail::valuewise::trichotomy;		//三分性		<all_of, combination_pair>		PresetEquivalenceRelation

}

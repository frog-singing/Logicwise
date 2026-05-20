// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "equivalence.h"


//同质关系::相容 homogeneous relation::tolerance================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//默认的相容关系
	using default_tolerance_relation_fn = default_equivalence_relation_fn;

	inline constexpr default_tolerance_relation_fn default_tolerance_relation{};

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//默认的相容关系
	using default_tolerance_relation_fn = default_equivalence_relation_fn;

	inline constexpr default_tolerance_relation_fn default_tolerance_relation{};

}


//逻辑维度::类型维度
namespace logicwise::typewise
{
	//默认的相容关系
	using detail::typewise::default_tolerance_relation;

	/*
	* 用户自定义的相容关系必须满足：
	* Customized tolerance relation must satisfy:
	*
	* 自反性 Reflexivity		: TolerantOf<T, T>() == true
	* 对称性 Symmetry		: TolerantOf<T1, T2>() == TolerantOf<T2, T1>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	//默认的相容关系
	using detail::valuewise::default_tolerance_relation;

	/*
	* 用户自定义的相容关系必须满足：
	* Customized tolerance relation must satisfy:
	*
	* 自反性 Reflexivity		: TolerantOf<V, V>() == true
	* 对称性 Symmetry		: TolerantOf<V1, V2>() == TolerantOf<V2, V1>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

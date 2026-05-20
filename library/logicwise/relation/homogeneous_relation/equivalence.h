// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/equivalence.h>
#include <type_traits> //用于 std::true_type


//同质关系::等价 homogeneous relation::equivalence================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//默认的等价关系
	struct default_equivalence_relation_fn
	{
		template<typename T1, typename T2>
		constexpr bool operator()() const { return typewise::same_as<T1, T2>; }

		using is_default = std::true_type;
	};

	inline constexpr default_equivalence_relation_fn default_equivalence_relation{};

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//默认的等价关系
	struct default_equivalence_relation_fn
	{
		template<auto V1, auto V2>
		constexpr bool operator()() const { return valuewise::equal_to<V1, V2>; }

		using is_default = std::true_type;
	};

	inline constexpr default_equivalence_relation_fn default_equivalence_relation{};

}


//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::same_as;

	//默认的等价关系
	using detail::typewise::default_equivalence_relation;

	/*
	* 用户自定义的等价关系必须满足：
	* Customized equivalence relation must satisfy:
	*
	* 自反性 Reflexivity		: SameAs<T, T>() == true
	* 对称性 Symmetry		: SameAs<T1, T2>() == SameAs<T2, T1>()
	* 传递性 Transitivity	: SameAs<T1, T2>() && SameAs<T2, T3>()  ==>  SameAs<T1, T3>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::same_as;
	using detail::valuewise::equal_to;

	//默认的等价关系
	using detail::valuewise::default_equivalence_relation;

	/*
	* 用户自定义的等价关系必须满足：
	* Customized equivalence relation must satisfy:
	*
	* 自反性 Reflexivity		: EqualTo<V, V>() == true
	* 对称性 Symmetry		: EqualTo<V1, V2>() == EqualTo<V2, V1>()
	* 传递性 Transitivity	: EqualTo<V1, V2>() && EqualTo<V2, V3>()  ==>  EqualTo<V1, V3>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

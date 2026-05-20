// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::derived_from, std::same_as，C++20标准
#include <type_traits> //用于 std::true_type


//同质关系::偏序 homogeneous relation::partial order================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//默认的偏序关系
	struct default_partial_order_fn
	{
		//非类类型在 std::derived_from 中不具备自反性，即 std::derived_from<int, int> == false 
		//而考虑到非类类型与其他类型在继承意义下不具备可比性，因此可以显式补上 || std::same_as<T1, T2>
		//即在继承意义下，非类类型的偏序关系退化为等价关系和不可比关系
		//注意对于类类型，std::derived_from<T1, T2> && std::derived_from<T2, T1> 并不能严格导出 std::same_as<T1, T2>
		//因为 std::same_as<T1, T2> 要求二者的 CV 限定符也相同
		template<typename T1, typename T2>
		constexpr bool operator()() const { return std::derived_from<T1, T2> || std::same_as<T1, T2>; }

		using is_default = std::true_type;
	};

	inline constexpr default_partial_order_fn default_partial_order{};

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//默认的偏序关系
	struct default_partial_order_fn
	{
		template<auto V1, auto V2>
		constexpr bool operator()() const { return requires { requires V1 <= V2; }; }

		using is_default = std::true_type;
	};

	inline constexpr default_partial_order_fn default_partial_order{};

	/*
	* 偏序关系 ≤ 必须满足：
	* Partial order ≤ must satisfy:
	*
	* 自反性	Reflexivity		: bool(V ≤ V) == true
	* 反对称性 Antisymmetry	: (V1 ≤ V2) && (V2 ≤ V1)  ==>  (V1 == V2)
	* 传递性 Transitivity	: (V1 ≤ V2) && (V2 ≤ V3)  ==>  (V1 ≤ V3)
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}


//逻辑维度::类型维度
namespace logicwise::typewise
{
	//默认的偏序关系
	using detail::typewise::default_partial_order;
	
	/*
	* 用户自定义的偏序关系必须满足：
	* Customized partial order must satisfy:
	*
	* 自反性 Reflexivity		: PartiallyOrdered<T, T>() == true
	* 反对称性 Antisymmetry	: PartiallyOrdered<T1, T2>() && PartiallyOrdered<T2, T1>()  ==>  T1 == T2
	* 传递性 Transitivity	: PartiallyOrdered<T1, T2>() && PartiallyOrdered<T2, T3>()  ==>  PartiallyOrdered<T1, T3>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	//默认的偏序关系
	using detail::valuewise::default_partial_order;

	/*
	* 用户自定义的偏序关系必须满足：
	* Customized partial order must satisfy:
	*
	* 自反性 Reflexivity		: PartiallyOrdered<V, V>() == true
	* 反对称性 Antisymmetry	: PartiallyOrdered<V1, V2>() && PartiallyOrdered<V2, V1>()  ==>  V1 == V2
	* 传递性 Transitivity	: PartiallyOrdered<V1, V2>() && PartiallyOrdered<V2, V3>()  ==>  PartiallyOrdered<V1, V3>()
	*
	* 否则视为未定义行为    Undefined behavior otherwise
	*/

}

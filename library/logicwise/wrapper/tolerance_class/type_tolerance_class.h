// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/rangewise.h>
#include <logicwise/arrangement.h>
#include <logicwise/relation/homogeneous_relation.h>
#include <logicwise/semantics/identity_guard.h>
#include "tag.h"
#include <concepts> //用于 std::derived_from，C++20标准
#include <type_traits> //用于 std::remove_cvref_t (C++20)


//包装器::相容类 wrapper::tolerance class================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 TolerantOf 均为类型维度

	//类型相容类约束
	template<typename ToleranceClass, auto TolerantOf = try_to_trust_existing_relation>
	concept ProvenToleranceClass =
		std::derived_from<ToleranceClass, type_tolerance_class_tag> &&
		(requires { ToleranceClass::tolerance_relation; }) &&
		(TryToTrustExistingRelation<TolerantOf> || valuewise::equal_to<ToleranceClass::tolerance_relation, TolerantOf>);

	template<typename Range, auto TolerantOf>
	concept ToleranceClassWitness =
		rangewise<all_of, combination_pair>
		::template in<Range>()
		.satisfies(TolerantOf);

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	concept ToleranceClass =
		ProvenToleranceClass<Range, TolerantOf> ||
		(TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, default_tolerance_relation>) ||
		(!TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, TolerantOf>);

	//类型相容类
	template<auto ToleranceRelation, typename... Type>
	struct tolerance_class
		: type_list<Type...>
		, type_tolerance_class_tag
	{
		static constexpr auto tolerance_relation{ ToleranceRelation };
	};

	template<auto ToleranceRelation, typename... Type>
		requires ToleranceClassWitness<type_list<Type...>, ToleranceRelation>
	struct tolerance_class_witness
	{
		using type = tolerance_class<ToleranceRelation, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型相容类工厂
	template<typename... Type>
	using type_tolerance_class = typename typewise::
		tolerance_class_witness<typewise::default_tolerance_relation, Type...>::type;

	template<auto TolerantOf, typename... Type>
	using custom_type_tolerance_class = typename typewise::
		tolerance_class_witness<TolerantOf, Type...>::type;

	//类型相容类适配器
	template<auto TolerantOf>
	struct type_tolerance_class_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_tolerance_class<TolerantOf, Type...>;
	};

	template<>
	struct type_tolerance_class_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_tolerance_class<Type...>;
	};

	template<typename Range, auto TolerantOf>
	struct as_type_tolerance_class_impl
	{
		template<typename... Type>
		using wrapper = type_tolerance_class_wrapper_selector<TolerantOf>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename ToleranceClass, auto TolerantOf>
		requires typewise::ProvenToleranceClass<ToleranceClass, TolerantOf>
	struct as_type_tolerance_class_impl<ToleranceClass, TolerantOf>
	{
		using type = ToleranceClass;
	};

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	using as_type_tolerance_class =
		typename as_type_tolerance_class_impl<std::remove_cvref_t<Range>, TolerantOf>::type;

	//受信任类型相容类适配器
	template<typename Range, auto TolerantOf>
	struct as_trusted_type_tolerance_class_impl
	{
		template<typename... Type>
		using wrapper = typewise::tolerance_class<TolerantOf, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto TolerantOf>
	using as_trusted_type_tolerance_class =
		typename as_trusted_type_tolerance_class_impl<Range, TolerantOf>::type;

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/rangewise.h>
#include <logicwise/arrangement.h>
#include <logicwise/relation/homogeneous_relation.h>
#include "tag.h"
#include <logicwise/detail/identity_guard.h>
#include <concepts> //用于 std::derived_from，C++20标准
#include <type_traits> //用于 std::remove_cvref_t


//包装器::相容集 wrapper::tolerance set================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 TolerantOf 均为类型维度

	//类型相容集约束
	template<typename ToleranceSet, auto TolerantOf = try_to_trust_existing_relation>
	concept ProvenToleranceSet =
		std::derived_from<ToleranceSet, type_tolerance_set_tag> &&
		(requires { ToleranceSet::tolerance_relation; }) &&
		(TryToTrustExistingRelation<TolerantOf> || valuewise::equal_to<ToleranceSet::tolerance_relation, TolerantOf>);

	template<typename Range, auto TolerantOf>
	concept ToleranceSetWitness =
		rangewise<none_of, combination_pair>
		::template in<Range>()
		.satisfies(TolerantOf);

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	concept ToleranceSet =
		ProvenToleranceSet<Range, TolerantOf> ||
		(TryToTrustExistingRelation<TolerantOf> && ToleranceSetWitness<Range, default_tolerance_relation>) ||
		(!TryToTrustExistingRelation<TolerantOf> && ToleranceSetWitness<Range, TolerantOf>);

	//类型相容集
	template<auto ToleranceRelation, typename... Type>
	struct tolerance_set
		: type_list<Type...>
		, type_tolerance_set_tag
	{
		static constexpr auto tolerance_relation{ ToleranceRelation };
	};

	template<auto ToleranceRelation, typename... Type>
		requires ToleranceSetWitness<type_list<Type...>, ToleranceRelation>
	struct tolerance_set_witness
	{
		using type = tolerance_set<ToleranceRelation, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型相容集工厂
	template<typename... Type>
	using type_tolerance_set = typename typewise::
		tolerance_set_witness<typewise::default_tolerance_relation, Type...>::type;

	template<auto TolerantOf, typename... Type>
	using custom_type_tolerance_set = typename typewise::
		tolerance_set_witness<TolerantOf, Type...>::type;

	//类型相容集适配器
	template<auto TolerantOf>
	struct type_tolerance_set_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_tolerance_set<TolerantOf, Type...>;
	};

	template<>
	struct type_tolerance_set_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_tolerance_set<Type...>;
	};

	template<typename Range, auto TolerantOf>
	struct as_type_tolerance_set_impl
	{
		template<typename... Type>
		using wrapper = type_tolerance_set_wrapper_selector<TolerantOf>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename ToleranceSet, auto TolerantOf>
		requires typewise::ProvenToleranceSet<ToleranceSet, TolerantOf>
	struct as_type_tolerance_set_impl<ToleranceSet, TolerantOf>
	{
		using type = ToleranceSet;
	};

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	using as_type_tolerance_set =
		typename as_type_tolerance_set_impl<std::remove_cvref_t<Range>, TolerantOf>::type;

	//受信任类型相容集适配器
	template<typename Range, auto TolerantOf>
	struct as_trusted_type_tolerance_set_impl
	{
		template<typename... Type>
		using wrapper = typewise::tolerance_set<TolerantOf, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto TolerantOf>
	using as_trusted_type_tolerance_set =
		typename as_trusted_type_tolerance_set_impl<Range, TolerantOf>::type;

}

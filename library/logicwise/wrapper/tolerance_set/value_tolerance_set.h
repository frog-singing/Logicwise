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
#include <type_traits> //用于 std::remove_cvref_t (C++20)


//包装器::相容集 wrapper::tolerance set================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 TolerantOf 均为值维度

	//值相容集约束
	template<typename ToleranceSet, auto TolerantOf = try_to_trust_existing_relation>
	concept ProvenToleranceSet =
		std::derived_from<ToleranceSet, value_tolerance_set_tag> &&
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

	//值相容集
	template<auto ToleranceRelation, auto... Value>
	struct tolerance_set
		: value_list<Value...>
		, value_tolerance_set_tag
	{
		static constexpr auto tolerance_relation{ ToleranceRelation };
	};

	template<auto ToleranceRelation, auto... Value>
		requires ToleranceSetWitness<value_list<Value...>, ToleranceRelation>
	struct tolerance_set_witness
	{
		using type = tolerance_set<ToleranceRelation, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值相容集工厂
	template<auto... Value>
	using value_tolerance_set = typename valuewise::
		tolerance_set_witness<valuewise::default_tolerance_relation, Value...>::type;

	template<auto TolerantOf, auto... Value>
	using custom_value_tolerance_set = typename valuewise::
		tolerance_set_witness<TolerantOf, Value...>::type;

	//值相容集适配器
	template<auto TolerantOf>
	struct value_tolerance_set_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_tolerance_set<TolerantOf, Value...>;
	};

	template<>
	struct value_tolerance_set_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_tolerance_set<Value...>;
	};

	template<typename Range, auto TolerantOf>
	struct as_value_tolerance_set_impl
	{
		template<auto... Value>
		using wrapper = value_tolerance_set_wrapper_selector<TolerantOf>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename ToleranceSet, auto TolerantOf>
		requires valuewise::ProvenToleranceSet<ToleranceSet, TolerantOf>
	struct as_value_tolerance_set_impl<ToleranceSet, TolerantOf>
	{
		using type = ToleranceSet;
	};

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	using as_value_tolerance_set =
		typename as_value_tolerance_set_impl<std::remove_cvref_t<Range>, TolerantOf>::type;

	//受信任值相容集适配器
	template<typename Range, auto TolerantOf>
	struct as_trusted_value_tolerance_set_impl
	{
		template<auto... Value>
		using wrapper = valuewise::tolerance_set<TolerantOf, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto TolerantOf>
	using as_trusted_value_tolerance_set =
		typename as_trusted_value_tolerance_set_impl<Range, TolerantOf>::type;

}

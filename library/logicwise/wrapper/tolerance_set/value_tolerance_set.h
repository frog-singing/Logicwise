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
#include "type.h"
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
		::in<Range>()
		.satisfies(TolerantOf);

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	concept ToleranceSet =
		ProvenToleranceSet<Range, TolerantOf> ||
		(TryToTrustExistingRelation<TolerantOf> && ToleranceSetWitness<Range, default_tolerance_relation>) ||
		(!TryToTrustExistingRelation<TolerantOf> && ToleranceSetWitness<Range, TolerantOf>);

	//值相容集包装器
	template<auto ToleranceRelation = try_to_trust_existing_relation>
	struct tolerance_set
	{
		template<auto... Value>
		struct wrapping
			: value_list<Value...>
			, value_tolerance_set_tag
		{
			static constexpr auto tolerance_relation{ ToleranceRelation };
		};

		template<auto... Value>
			requires ToleranceSetWitness<value_list<Value...>, ToleranceRelation>
		using of = wrapping<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceSet<Range, ToleranceRelation>;
	};

	template<>
	struct tolerance_set<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapping = tolerance_set<default_tolerance_relation>::wrapping<Value...>;

		template<auto... Value>
		using of = tolerance_set<default_tolerance_relation>::of<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceSet<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值相容集工厂
	template<auto... Value>
	using value_tolerance_set = valuewise::tolerance_set<>::of<Value...>;

}

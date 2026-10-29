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


//包装器::相容类 wrapper::tolerance class================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 TolerantOf 均为值维度

	//值相容类约束
	template<typename ToleranceClass, auto TolerantOf = try_to_trust_existing_relation>
	concept ProvenToleranceClass =
		std::derived_from<ToleranceClass, value_tolerance_class_tag> &&
		(requires { ToleranceClass::tolerance_relation; }) &&
		(TryToTrustExistingRelation<TolerantOf> || valuewise::equal_to<ToleranceClass::tolerance_relation, TolerantOf>);

	template<typename Range, auto TolerantOf>
	concept ToleranceClassWitness =
		rangewise<all_of, combination_pair>
		::in<Range>()
		.satisfies(TolerantOf);

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	concept ToleranceClass =
		ProvenToleranceClass<Range, TolerantOf> ||
		(TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, default_tolerance_relation>) ||
		(!TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, TolerantOf>);

	//值相容类包装器
	template<auto ToleranceRelation = try_to_trust_existing_relation>
	struct tolerance_class
	{
		template<auto... Value>
		struct wrapping
			: value_list<Value...>
			, value_tolerance_class_tag
		{
			static constexpr auto tolerance_relation{ ToleranceRelation };
		};

		template<auto... Value>
			requires ToleranceClassWitness<value_list<Value...>, ToleranceRelation>
		using of = wrapping<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceClass<Range, ToleranceRelation>;
	};

	template<>
	struct tolerance_class<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapping = tolerance_class<default_tolerance_relation>::wrapping<Value...>;

		template<auto... Value>
		using of = tolerance_class<default_tolerance_relation>::of<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceClass<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值相容类工厂
	template<auto... Value>
	using value_tolerance_class = valuewise::tolerance_class<>::of<Value...>;

}

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
		::in<Range>()
		.satisfies(TolerantOf);

	template<typename Range, auto TolerantOf = try_to_trust_existing_relation>
	concept ToleranceClass =
		ProvenToleranceClass<Range, TolerantOf> ||
		(TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, default_tolerance_relation>) ||
		(!TryToTrustExistingRelation<TolerantOf> && ToleranceClassWitness<Range, TolerantOf>);

	//类型相容类包装器
	template<auto ToleranceRelation = try_to_trust_existing_relation>
	struct tolerance_class
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_tolerance_class_tag
		{
			static constexpr auto tolerance_relation{ ToleranceRelation };
		};

		template<typename... Type>
			requires ToleranceClassWitness<type_list<Type...>, ToleranceRelation>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceClass<Range, ToleranceRelation>;
	};

	template<>
	struct tolerance_class<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = tolerance_class<default_tolerance_relation>::wrapping<Type...>;

		template<typename... Type>
		using of = tolerance_class<default_tolerance_relation>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenToleranceClass<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型相容类工厂
	template<typename... Type>
	using type_tolerance_class = typewise::tolerance_class<>::of<Type...>;

}

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


//包装器::等价类 wrapper::equivalence class================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 EqualTo 均为值维度

	//值等价类约束
	template<typename EquivalenceClass, auto EqualTo = try_to_trust_existing_relation>
	concept ProvenEquivalenceClass =
		std::derived_from<EquivalenceClass, value_equivalence_class_tag> &&
		(requires { EquivalenceClass::equivalence_relation; }) &&
		(TryToTrustExistingRelation<EqualTo> || valuewise::equal_to<EquivalenceClass::equivalence_relation, EqualTo>);

	template<typename Range, auto EqualTo>
	concept EquivalenceClassWitness =
		rangewise<all_of, linear_adjacent_pair>
		::in<Range>()
		.satisfies(EqualTo);

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	concept EquivalenceClass =
		ProvenEquivalenceClass<Range, EqualTo> ||
		(TryToTrustExistingRelation<EqualTo> && EquivalenceClassWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<EqualTo> && EquivalenceClassWitness<Range, EqualTo>);

	//值等价类包装器
	template<auto EquivalenceRelation = try_to_trust_existing_relation>
	struct equivalence_class
	{
		template<auto... Value>
		struct wrapping
			: value_list<Value...>
			, value_equivalence_class_tag
			, equivalence_relation_induction<EquivalenceRelation>
		{
			static constexpr auto equivalence_relation{ EquivalenceRelation };
		};

		template<auto... Value>
			requires EquivalenceClassWitness<value_list<Value...>, EquivalenceRelation>
		using of = wrapping<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenEquivalenceClass<Range, EquivalenceRelation>;
	};

	template<>
	struct equivalence_class<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapping = equivalence_class<default_equivalence_relation>::wrapping<Value...>;

		template<auto... Value>
		using of = equivalence_class<default_equivalence_relation>::of<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenEquivalenceClass<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值等价类工厂
	template<auto... Value>
	using value_equivalence_class = valuewise::equivalence_class<>::of<Value...>;

}

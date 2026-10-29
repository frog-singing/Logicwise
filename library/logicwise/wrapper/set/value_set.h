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


//包装器::集合 wrapper::set================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 EqualTo 均为值维度

	//值集合约束
	template<typename Set, auto EqualTo = try_to_trust_existing_relation>
	concept ProvenSet =
		std::derived_from<Set, value_set_tag> &&
		(requires { Set::equivalence_relation; }) &&
		(TryToTrustExistingRelation<EqualTo> || valuewise::equal_to<Set::equivalence_relation, EqualTo>);

	template<typename Range, auto EqualTo>
	concept SetWitness =
		rangewise<none_of, combination_pair>
		::in<Range>()
		.satisfies(EqualTo);

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	concept Set =
		ProvenSet<Range, EqualTo> ||
		(TryToTrustExistingRelation<EqualTo> && SetWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<EqualTo> && SetWitness<Range, EqualTo>);

	//值集合包装器
	template<auto EquivalenceRelation = try_to_trust_existing_relation>
	struct set
	{
		template<auto... Value>
		struct wrapping
			: value_list<Value...>
			, value_set_tag
			, equivalence_relation_induction<EquivalenceRelation>
		{
			static constexpr auto equivalence_relation{ EquivalenceRelation };
		};

		template<auto... Value>
			requires SetWitness<value_list<Value...>, EquivalenceRelation>
		using of = wrapping<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenSet<Range, EquivalenceRelation>;
	};

	template<>
	struct set<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapping = set<default_equivalence_relation>::wrapping<Value...>;

		template<auto... Value>
		using of = set<default_equivalence_relation>::of<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenSet<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值集合工厂
	template<auto... Value>
	using value_set = valuewise::set<>::of<Value...>;

}

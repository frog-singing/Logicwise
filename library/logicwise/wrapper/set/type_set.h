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

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 SameAs 均为类型维度

	//类型集合约束
	template<typename Set, auto SameAs = try_to_trust_existing_relation>
	concept ProvenSet =
		std::derived_from<Set, type_set_tag> &&
		(requires { Set::equivalence_relation; }) &&
		(TryToTrustExistingRelation<SameAs> || valuewise::equal_to<Set::equivalence_relation, SameAs>);

	template<typename Range, auto SameAs>
	concept SetWitness =
		rangewise<none_of, combination_pair>
		::in<Range>()
		.satisfies(SameAs);

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	concept Set =
		ProvenSet<Range, SameAs> ||
		(TryToTrustExistingRelation<SameAs> && SetWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<SameAs> && SetWitness<Range, SameAs>);

	//类型集合包装器
	template<auto EquivalenceRelation = try_to_trust_existing_relation>
	struct set
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_set_tag
			, equivalence_relation_induction<EquivalenceRelation>
		{
			static constexpr auto equivalence_relation{ EquivalenceRelation };
		};

		template<typename... Type>
			requires SetWitness<type_list<Type...>, EquivalenceRelation>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenSet<Range, EquivalenceRelation>;
	};

	template<>
	struct set<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = set<default_equivalence_relation>::wrapping<Type...>;

		template<typename... Type>
		using of = set<default_equivalence_relation>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenSet<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型集合工厂
	template<typename... Type>
	using type_set = typewise::set<>::of<Type...>;

}

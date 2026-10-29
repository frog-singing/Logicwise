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

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 SameAs 均为类型维度

	//类型等价类约束
	template<typename EquivalenceClass, auto SameAs = try_to_trust_existing_relation>
	concept ProvenEquivalenceClass =
		std::derived_from<EquivalenceClass, type_equivalence_class_tag> &&
		(requires { EquivalenceClass::equivalence_relation; }) &&
		(TryToTrustExistingRelation<SameAs> || valuewise::equal_to<EquivalenceClass::equivalence_relation, SameAs>);

	template<typename Range, auto SameAs>
	concept EquivalenceClassWitness =
		rangewise<all_of, linear_adjacent_pair>
		::in<Range>()
		.satisfies(SameAs);

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	concept EquivalenceClass =
		ProvenEquivalenceClass<Range, SameAs> ||
		(TryToTrustExistingRelation<SameAs> && EquivalenceClassWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<SameAs> && EquivalenceClassWitness<Range, SameAs>);

	//类型等价类包装器
	template<auto EquivalenceRelation = try_to_trust_existing_relation>
	struct equivalence_class
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_equivalence_class_tag
			, equivalence_relation_induction<EquivalenceRelation>
		{
			static constexpr auto equivalence_relation{ EquivalenceRelation };
		};

		template<typename... Type>
			requires EquivalenceClassWitness<type_list<Type...>, EquivalenceRelation>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenEquivalenceClass<Range, EquivalenceRelation>;
	};

	template<>
	struct equivalence_class<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = equivalence_class<default_equivalence_relation>::wrapping<Type...>;

		template<typename... Type>
		using of = equivalence_class<default_equivalence_relation>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenEquivalenceClass<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型等价类工厂
	template<typename... Type>
	using type_equivalence_class = typewise::equivalence_class<>::of<Type...>;

}

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


//包装器::偏序集 wrapper::partially ordered set================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 PartialOrder 均为类型维度
	//同时要求偏序集按拓扑序排列

	//类型偏序集约束
	template<typename Poset, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenPoset =
		std::derived_from<Poset, type_poset_tag> &&
		(requires { Poset::partial_order; }) &&
		(TryToTrustExistingRelation<PartialOrder> || valuewise::equal_to<Poset::partial_order, PartialOrder>);

	template<typename Range, auto PartialOrder>
	concept PosetWitness =
		rangewise<none_of, combination_pair>
		::in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::dual_partial_order);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Poset =
		ProvenPoset<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, PartialOrder>);

	//类型偏序集包装器
	template<auto PartialOrder = try_to_trust_existing_relation>
	struct poset
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_poset_tag
			, partial_order_induction<PartialOrder>
		{
			static constexpr auto partial_order{ PartialOrder };
		};

		template<typename... Type>
			requires PosetWitness<type_list<Type...>, PartialOrder>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenPoset<Range, PartialOrder>;
	};

	template<>
	struct poset<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = poset<default_partial_order>::wrapping<Type...>;

		template<typename... Type>
		using of = poset<default_partial_order>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenPoset<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型偏序集工厂
	template<typename... Type>
	using type_poset = typewise::poset<>::of<Type...>;

}

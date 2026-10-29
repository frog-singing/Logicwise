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


//包装器::链 wrapper::chain================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 PartialOrder 均为类型维度
	//同时要求链按拓扑序排列

	//类型链约束
	template<typename Chain, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenChain =
		std::derived_from<Chain, type_chain_tag> &&
		(requires { Chain::partial_order; }) &&
		(TryToTrustExistingRelation<PartialOrder> || valuewise::equal_to<Chain::partial_order, PartialOrder>);

	template<typename Range, auto PartialOrder>
	concept ChainWitness =
		rangewise<all_of, linear_adjacent_pair>
		::in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::strict_partial_order);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Chain =
		ProvenChain<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && ChainWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && ChainWitness<Range, PartialOrder>);

	//类型链包装器
	template<auto PartialOrder = try_to_trust_existing_relation>
	struct chain
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_chain_tag
			, partial_order_induction<PartialOrder>
		{
			static constexpr auto partial_order{ PartialOrder };
		};

		template<typename... Type>
			requires ChainWitness<type_list<Type...>, PartialOrder>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenChain<Range, PartialOrder>;
	};

	template<>
	struct chain<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = chain<default_partial_order>::wrapping<Type...>;

		template<typename... Type>
		using of = chain<default_partial_order>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenChain<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型链工厂
	template<typename... Type>
	using type_chain = typewise::chain<>::of<Type...>;

}

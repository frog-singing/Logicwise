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
#include <type_traits> //用于 std::remove_cvref_t


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
		::template in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::strict_partial_order);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Chain =
		ProvenChain<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && ChainWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && ChainWitness<Range, PartialOrder>);

	//类型链
	template<auto PartialOrder, typename... Type>
	struct chain
		: type_list<Type...>
		, type_chain_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, typename... Type>
		requires ChainWitness<type_list<Type...>, PartialOrder>
	struct chain_witness
	{
		using type = chain<PartialOrder, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型链工厂
	template<typename... Type>
	using type_chain = typename typewise::
		chain_witness<typewise::default_partial_order, Type...>::type;

	template<auto PartialOrder, typename... Type>
	using custom_type_chain = typename typewise::
		chain_witness<PartialOrder, Type...>::type;

	//类型链适配器
	template<auto PartialOrder>
	struct type_chain_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_chain<PartialOrder, Type...>;
	};

	template<>
	struct type_chain_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_chain<Type...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_type_chain_impl
	{
		template<typename... Type>
		using wrapper = type_chain_wrapper_selector<PartialOrder>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Chain, auto PartialOrder>
		requires typewise::ProvenChain<Chain, PartialOrder>
	struct as_type_chain_impl<Chain, PartialOrder>
	{
		using type = Chain;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_type_chain =
		typename as_type_chain_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任类型链适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_type_chain_impl
	{
		template<typename... Type>
		using wrapper = typewise::chain<PartialOrder, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_type_chain =
		typename as_trusted_type_chain_impl<Range, PartialOrder>::type;

}

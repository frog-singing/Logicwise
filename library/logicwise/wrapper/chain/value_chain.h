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
#include "tag.h"
#include <concepts> //用于 std::derived_from，C++20标准
#include <type_traits> //用于 std::remove_cvref_t (C++20)


//包装器::链 wrapper::chain================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 PartialOrder 均为值维度
	//同时要求链按拓扑序排列

	//值链约束
	template<typename Chain, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenChain =
		std::derived_from<Chain, value_chain_tag> &&
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

	//值链
	template<auto PartialOrder, auto... Value>
	struct chain
		: value_list<Value...>
		, value_chain_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, auto... Value>
		requires ChainWitness<value_list<Value...>, PartialOrder>
	struct chain_witness
	{
		using type = chain<PartialOrder, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值链工厂
	template<auto... Value>
	using value_chain = typename valuewise::
		chain_witness<valuewise::default_partial_order, Value...>::type;

	template<auto PartialOrder, auto... Value>
	using custom_value_chain = typename valuewise::
		chain_witness<PartialOrder, Value...>::type;

	//值链适配器
	template<auto PartialOrder>
	struct value_chain_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_chain<PartialOrder, Value...>;
	};

	template<>
	struct value_chain_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_chain<Value...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_value_chain_impl
	{
		template<auto... Value>
		using wrapper = value_chain_wrapper_selector<PartialOrder>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Chain, auto PartialOrder>
		requires valuewise::ProvenChain<Chain, PartialOrder>
	struct as_value_chain_impl<Chain, PartialOrder>
	{
		using type = Chain;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_value_chain =
		typename as_value_chain_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任值链适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_value_chain_impl
	{
		template<auto... Value>
		using wrapper = valuewise::chain<PartialOrder, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_value_chain =
		typename as_trusted_value_chain_impl<Range, PartialOrder>::type;

}

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


//包装器::偏序集 wrapper::partially ordered set================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 PartialOrder 均为值维度
	//同时要求偏序集按拓扑序排列

	//值偏序集约束
	template<typename Poset, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenPoset =
		std::derived_from<Poset, value_poset_tag> &&
		(requires { Poset::partial_order; }) &&
		(TryToTrustExistingRelation<PartialOrder> || valuewise::equal_to<Poset::partial_order, PartialOrder>);

	template<typename Range, auto PartialOrder>
	concept PosetWitness =
		rangewise<none_of, combination_pair>
		::template in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::dual_partial_order);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Poset =
		ProvenPoset<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, PartialOrder>);

	//值偏序集
	template<auto PartialOrder, auto... Value>
	struct poset
		: value_list<Value...>
		, value_poset_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, auto... Value>
		requires PosetWitness<value_list<Value...>, PartialOrder>
	struct poset_witness
	{
		using type = poset<PartialOrder, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值偏序集工厂
	template<auto... Value>
	using value_poset = typename valuewise::
		poset_witness<valuewise::default_partial_order, Value...>::type;

	template<auto PartialOrder, auto... Value>
	using custom_value_poset = typename valuewise::
		poset_witness<PartialOrder, Value...>::type;

	//值偏序集适配器
	template<auto PartialOrder>
	struct value_poset_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_poset<PartialOrder, Value...>;
	};

	template<>
	struct value_poset_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_poset<Value...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_value_poset_impl
	{
		template<auto... Value>
		using wrapper = value_poset_wrapper_selector<PartialOrder>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Poset, auto PartialOrder>
		requires valuewise::ProvenPoset<Poset, PartialOrder>
	struct as_value_poset_impl<Poset, PartialOrder>
	{
		using type = Poset;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_value_poset =
		typename as_value_poset_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任值偏序集适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_value_poset_impl
	{
		template<auto... Value>
		using wrapper = valuewise::poset<PartialOrder, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_value_poset =
		typename as_trusted_value_poset_impl<Range, PartialOrder>::type;

}

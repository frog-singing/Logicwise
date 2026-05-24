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
#include <type_traits> //用于 std::remove_cvref_t (C++20)


//包装器::反链 wrapper::antichain================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 PartialOrder 均为值维度

	//值反链约束
	template<typename Antichain, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenAntichain =
		std::derived_from<Antichain, value_antichain_tag> &&
		(requires { Antichain::partial_order; }) &&
		(TryToTrustExistingRelation<PartialOrder> || valuewise::equal_to<Antichain::partial_order, PartialOrder>);

	template<typename Range, auto PartialOrder>
	concept AntichainWitness =
		rangewise<all_of, combination_pair>
		::template in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::incomparability_relation);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Antichain =
		ProvenAntichain<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, PartialOrder>);

	//值反链
	template<auto PartialOrder, auto... Value>
	struct antichain
		: value_list<Value...>
		, value_antichain_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, auto... Value>
		requires AntichainWitness<value_list<Value...>, PartialOrder>
	struct antichain_witness
	{
		using type = antichain<PartialOrder, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值反链工厂
	template<auto... Value>
	using value_antichain = typename valuewise::
		antichain_witness<valuewise::default_partial_order, Value...>::type;

	template<auto PartialOrder, auto... Value>
	using custom_value_antichain = typename valuewise::
		antichain_witness<PartialOrder, Value...>::type;

	//值反链适配器
	template<auto PartialOrder>
	struct value_antichain_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_antichain<PartialOrder, Value...>;
	};

	template<>
	struct value_antichain_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_antichain<Value...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_value_antichain_impl
	{
		template<auto... Value>
		using wrapper = value_antichain_wrapper_selector<PartialOrder>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Antichain, auto PartialOrder>
		requires valuewise::ProvenAntichain<Antichain, PartialOrder>
	struct as_value_antichain_impl<Antichain, PartialOrder>
	{
		using type = Antichain;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_value_antichain =
		typename as_value_antichain_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任值反链适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_value_antichain_impl
	{
		template<auto... Value>
		using wrapper = valuewise::antichain<PartialOrder, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_value_antichain =
		typename as_trusted_value_antichain_impl<Range, PartialOrder>::type;

}

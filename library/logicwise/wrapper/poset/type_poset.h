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
		::template in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::dual_partial_order);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Poset =
		ProvenPoset<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && PosetWitness<Range, PartialOrder>);

	//类型偏序集
	template<auto PartialOrder, typename... Type>
	struct poset
		: type_list<Type...>
		, type_poset_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, typename... Type>
		requires PosetWitness<type_list<Type...>, PartialOrder>
	struct poset_witness
	{
		using type = poset<PartialOrder, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型偏序集工厂
	template<typename... Type>
	using type_poset = typename typewise::
		poset_witness<typewise::default_partial_order, Type...>::type;

	template<auto PartialOrder, typename... Type>
	using custom_type_poset = typename typewise::
		poset_witness<PartialOrder, Type...>::type;

	//类型偏序集适配器
	template<auto PartialOrder>
	struct type_poset_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_poset<PartialOrder, Type...>;
	};

	template<>
	struct type_poset_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_poset<Type...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_type_poset_impl
	{
		template<typename... Type>
		using wrapper = type_poset_wrapper_selector<PartialOrder>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Poset, auto PartialOrder>
		requires typewise::ProvenPoset<Poset, PartialOrder>
	struct as_type_poset_impl<Poset, PartialOrder>
	{
		using type = Poset;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_type_poset =
		typename as_type_poset_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任类型偏序集适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_type_poset_impl
	{
		template<typename... Type>
		using wrapper = typewise::poset<PartialOrder, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_type_poset =
		typename as_trusted_type_poset_impl<Range, PartialOrder>::type;

}

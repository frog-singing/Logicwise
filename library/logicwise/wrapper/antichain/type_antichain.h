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


//包装器::反链 wrapper::antichain================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 PartialOrder 均为类型维度

	//类型反链约束
	template<typename Antichain, auto PartialOrder = try_to_trust_existing_relation>
	concept ProvenAntichain =
		std::derived_from<Antichain, type_antichain_tag> &&
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

	//类型反链
	template<auto PartialOrder, typename... Type>
	struct antichain
		: type_list<Type...>
		, type_antichain_tag
		, partial_order_induction<PartialOrder>
	{
		static constexpr auto partial_order{ PartialOrder };
	};

	template<auto PartialOrder, typename... Type>
		requires AntichainWitness<type_list<Type...>, PartialOrder>
	struct antichain_witness
	{
		using type = antichain<PartialOrder, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型反链工厂
	template<typename... Type>
	using type_antichain = typename typewise::
		antichain_witness<typewise::default_partial_order, Type...>::type;

	template<auto PartialOrder, typename... Type>
	using custom_type_antichain = typename typewise::
		antichain_witness<PartialOrder, Type...>::type;

	//类型反链适配器
	template<auto PartialOrder>
	struct type_antichain_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_antichain<PartialOrder, Type...>;
	};

	template<>
	struct type_antichain_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_antichain<Type...>;
	};

	template<typename Range, auto PartialOrder>
	struct as_type_antichain_impl
	{
		template<typename... Type>
		using wrapper = type_antichain_wrapper_selector<PartialOrder>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Antichain, auto PartialOrder>
		requires typewise::ProvenAntichain<Antichain, PartialOrder>
	struct as_type_antichain_impl<Antichain, PartialOrder>
	{
		using type = Antichain;
	};

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	using as_type_antichain =
		typename as_type_antichain_impl<std::remove_cvref_t<Range>, PartialOrder>::type;

	//受信任类型反链适配器
	template<typename Range, auto PartialOrder>
	struct as_trusted_type_antichain_impl
	{
		template<typename... Type>
		using wrapper = typewise::antichain<PartialOrder, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto PartialOrder>
	using as_trusted_type_antichain =
		typename as_trusted_type_antichain_impl<Range, PartialOrder>::type;

}

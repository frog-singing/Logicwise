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


//包装器::集合 wrapper::set================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 EqualTo 均为值维度

	//值集合约束
	template<typename Set, auto EqualTo = try_to_trust_existing_relation>
	concept ProvenSet =
		std::derived_from<Set, value_set_tag> &&
		(requires { Set::equivalence_relation; }) &&
		(TryToTrustExistingRelation<EqualTo> || valuewise::equal_to<Set::equivalence_relation, EqualTo>);

	template<typename Range, auto EqualTo>
	concept SetWitness =
		rangewise<none_of, combination_pair>
		::template in<Range>()
		.satisfies(EqualTo);

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	concept Set =
		ProvenSet<Range, EqualTo> ||
		(TryToTrustExistingRelation<EqualTo> && SetWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<EqualTo> && SetWitness<Range, EqualTo>);

	//值集合
	template<auto EquivalenceRelation, auto... Value>
	struct set
		: value_list<Value...>
		, value_set_tag
		, equivalence_relation_induction<EquivalenceRelation>
	{
		static constexpr auto equivalence_relation{ EquivalenceRelation };
	};

	template<auto EquivalenceRelation, auto... Value>
		requires SetWitness<value_list<Value...>, EquivalenceRelation>
	struct set_witness
	{
		using type = set<EquivalenceRelation, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值集合工厂
	template<auto... Value>
	using value_set = typename valuewise::
		set_witness<valuewise::default_equivalence_relation, Value...>::type;

	template<auto EqualTo, auto... Value>
	using custom_value_set = typename valuewise::
		set_witness<EqualTo, Value...>::type;

	//值集合适配器
	template<auto EqualTo>
	struct value_set_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_set<EqualTo, Value...>;
	};

	template<>
	struct value_set_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_set<Value...>;
	};

	template<typename Range, auto EqualTo>
	struct as_value_set_impl
	{
		template<auto... Value>
		using wrapper = value_set_wrapper_selector<EqualTo>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Set, auto EqualTo>
		requires valuewise::ProvenSet<Set, EqualTo>
	struct as_value_set_impl<Set, EqualTo>
	{
		using type = Set;
	};

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	using as_value_set =
		typename as_value_set_impl<std::remove_cvref_t<Range>, EqualTo>::type;

	//受信任值集合适配器
	template<typename Range, auto EqualTo>
	struct as_trusted_value_set_impl
	{
		template<auto... Value>
		using wrapper = valuewise::set<EqualTo, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto EqualTo>
	using as_trusted_value_set =
		typename as_trusted_value_set_impl<Range, EqualTo>::type;

}

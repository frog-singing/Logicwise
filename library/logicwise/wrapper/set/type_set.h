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

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 SameAs 均为类型维度

	//类型集合约束
	template<typename Set, auto SameAs = try_to_trust_existing_relation>
	concept ProvenSet =
		std::derived_from<Set, type_set_tag> &&
		(requires { Set::equivalence_relation; }) &&
		(TryToTrustExistingRelation<SameAs> || valuewise::equal_to<Set::equivalence_relation, SameAs>);

	template<typename Range, auto SameAs>
	concept SetWitness =
		rangewise<none_of, combination_pair>
		::template in<Range>()
		.satisfies(SameAs);

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	concept Set =
		ProvenSet<Range, SameAs> ||
		(TryToTrustExistingRelation<SameAs> && SetWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<SameAs> && SetWitness<Range, SameAs>);

	//类型集合
	template<auto EquivalenceRelation, typename... Type>
	struct set
		: type_list<Type...>
		, type_set_tag
		, equivalence_relation_induction<EquivalenceRelation>
	{
		static constexpr auto equivalence_relation{ EquivalenceRelation };
	};

	template<auto EquivalenceRelation, typename... Type>
		requires SetWitness<type_list<Type...>, EquivalenceRelation>
	struct set_witness
	{
		using type = set<EquivalenceRelation, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型集合工厂
	template<typename... Type>
	using type_set = typename typewise::
		set_witness<typewise::default_equivalence_relation, Type...>::type;

	template<auto SameAs, typename... Type>
	using custom_type_set = typename typewise::
		set_witness<SameAs, Type...>::type;

	//类型集合适配器
	template<auto SameAs>
	struct type_set_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_set<SameAs, Type...>;
	};

	template<>
	struct type_set_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_set<Type...>;
	};

	template<typename Range, auto SameAs>
	struct as_type_set_impl
	{
		template<typename... Type>
		using wrapper = type_set_wrapper_selector<SameAs>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Set, auto SameAs>
		requires typewise::ProvenSet<Set, SameAs>
	struct as_type_set_impl<Set, SameAs>
	{
		using type = Set;
	};

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	using as_type_set =
		typename as_type_set_impl<std::remove_cvref_t<Range>, SameAs>::type;

	//受信任类型集合适配器
	template<typename Range, auto SameAs>
	struct as_trusted_type_set_impl
	{
		template<typename... Type>
		using wrapper = typewise::set<SameAs, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto SameAs>
	using as_trusted_type_set =
		typename as_trusted_type_set_impl<Range, SameAs>::type;

}

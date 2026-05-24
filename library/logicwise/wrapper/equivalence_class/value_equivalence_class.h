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


//包装器::等价类 wrapper::equivalence class================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//确保 Range 和 EqualTo 均为值维度

	//值等价类约束
	template<typename EquivalenceClass, auto EqualTo = try_to_trust_existing_relation>
	concept ProvenEquivalenceClass =
		std::derived_from<EquivalenceClass, value_equivalence_class_tag> &&
		(requires { EquivalenceClass::equivalence_relation; }) &&
		(TryToTrustExistingRelation<EqualTo> || valuewise::equal_to<EquivalenceClass::equivalence_relation, EqualTo>);

	template<typename Range, auto EqualTo>
	concept EquivalenceClassWitness =
		rangewise<all_of, linear_adjacent_pair>
		::template in<Range>()
		.satisfies(EqualTo);

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	concept EquivalenceClass =
		ProvenEquivalenceClass<Range, EqualTo> ||
		(TryToTrustExistingRelation<EqualTo> && EquivalenceClassWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<EqualTo> && EquivalenceClassWitness<Range, EqualTo>);

	//值等价类
	template<auto EquivalenceRelation, auto... Value>
	struct equivalence_class
		: value_list<Value...>
		, value_equivalence_class_tag
		, equivalence_relation_induction<EquivalenceRelation>
	{
		static constexpr auto equivalence_relation{ EquivalenceRelation };
	};

	template<auto EquivalenceRelation, auto... Value>
		requires EquivalenceClassWitness<value_list<Value...>, EquivalenceRelation>
	struct equivalence_class_witness
	{
		using type = equivalence_class<EquivalenceRelation, Value...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值等价类工厂
	template<auto... Value>
	using value_equivalence_class = typename valuewise::
		equivalence_class_witness<valuewise::default_equivalence_relation, Value...>::type;

	template<auto EqualTo, auto... Value>
	using custom_value_equivalence_class = typename valuewise::
		equivalence_class_witness<EqualTo, Value...>::type;

	//值等价类适配器
	template<auto EqualTo>
	struct value_equivalence_class_wrapper_selector
	{
		template<auto... Value>
		using wrapper = custom_value_equivalence_class<EqualTo, Value...>;
	};

	template<>
	struct value_equivalence_class_wrapper_selector<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapper = value_equivalence_class<Value...>;
	};

	template<typename Range, auto EqualTo>
	struct as_value_equivalence_class_impl
	{
		template<auto... Value>
		using wrapper = value_equivalence_class_wrapper_selector<EqualTo>::template wrapper<Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename EquivalenceClass, auto EqualTo>
		requires valuewise::ProvenEquivalenceClass<EquivalenceClass, EqualTo>
	struct as_value_equivalence_class_impl<EquivalenceClass, EqualTo>
	{
		using type = EquivalenceClass;
	};

	template<typename Range, auto EqualTo = try_to_trust_existing_relation>
	using as_value_equivalence_class =
		typename as_value_equivalence_class_impl<std::remove_cvref_t<Range>, EqualTo>::type;

	//受信任值等价类适配器
	template<typename Range, auto EqualTo>
	struct as_trusted_value_equivalence_class_impl
	{
		template<auto... Value>
		using wrapper = valuewise::equivalence_class<EqualTo, Value...>;

		using type = typename as_value_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto EqualTo>
	using as_trusted_value_equivalence_class =
		typename as_trusted_value_equivalence_class_impl<Range, EqualTo>::type;

}

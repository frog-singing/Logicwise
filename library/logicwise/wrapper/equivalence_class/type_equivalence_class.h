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

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//确保 Range 和 SameAs 均为类型维度

	//类型等价类约束
	template<typename EquivalenceClass, auto SameAs = try_to_trust_existing_relation>
	concept ProvenEquivalenceClass =
		std::derived_from<EquivalenceClass, type_equivalence_class_tag> &&
		(requires { EquivalenceClass::equivalence_relation; }) &&
		(TryToTrustExistingRelation<SameAs> || valuewise::equal_to<EquivalenceClass::equivalence_relation, SameAs>);

	template<typename Range, auto SameAs>
	concept EquivalenceClassWitness =
		rangewise<all_of, linear_adjacent_pair>
		::template in<Range>()
		.satisfies(SameAs);

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	concept EquivalenceClass =
		ProvenEquivalenceClass<Range, SameAs> ||
		(TryToTrustExistingRelation<SameAs> && EquivalenceClassWitness<Range, default_equivalence_relation>) ||
		(!TryToTrustExistingRelation<SameAs> && EquivalenceClassWitness<Range, SameAs>);

	//类型等价类
	template<auto EquivalenceRelation, typename... Type>
	struct equivalence_class
		: type_list<Type...>
		, type_equivalence_class_tag
		, equivalence_relation_induction<EquivalenceRelation>
	{
		static constexpr auto equivalence_relation{ EquivalenceRelation };
	};

	template<auto EquivalenceRelation, typename... Type>
		requires EquivalenceClassWitness<type_list<Type...>, EquivalenceRelation>
	struct equivalence_class_witness
	{
		using type = equivalence_class<EquivalenceRelation, Type...>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型等价类工厂
	template<typename... Type>
	using type_equivalence_class = typename typewise::
		equivalence_class_witness<typewise::default_equivalence_relation, Type...>::type;

	template<auto SameAs, typename... Type>
	using custom_type_equivalence_class = typename typewise::
		equivalence_class_witness<SameAs, Type...>::type;

	//类型等价类适配器
	template<auto SameAs>
	struct type_equivalence_class_wrapper_selector
	{
		template<typename... Type>
		using wrapper = custom_type_equivalence_class<SameAs, Type...>;
	};

	template<>
	struct type_equivalence_class_wrapper_selector<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapper = type_equivalence_class<Type...>;
	};

	template<typename Range, auto SameAs>
	struct as_type_equivalence_class_impl
	{
		template<typename... Type>
		using wrapper = type_equivalence_class_wrapper_selector<SameAs>::template wrapper<Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename EquivalenceClass, auto SameAs>
		requires typewise::ProvenEquivalenceClass<EquivalenceClass, SameAs>
	struct as_type_equivalence_class_impl<EquivalenceClass, SameAs>
	{
		using type = EquivalenceClass;
	};

	template<typename Range, auto SameAs = try_to_trust_existing_relation>
	using as_type_equivalence_class =
		typename as_type_equivalence_class_impl<std::remove_cvref_t<Range>, SameAs>::type;

	//受信任类型等价类适配器
	template<typename Range, auto SameAs>
	struct as_trusted_type_equivalence_class_impl
	{
		template<typename... Type>
		using wrapper = typewise::equivalence_class<SameAs, Type...>;

		using type = typename as_type_list<Range>::template apply<wrapper>;
	};

	template<typename Range, auto SameAs>
	using as_trusted_type_equivalence_class =
		typename as_trusted_type_equivalence_class_impl<Range, SameAs>::type;

}

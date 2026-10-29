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
#include "type.h"
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
		::in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::incomparability_relation);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Antichain =
		ProvenAntichain<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, PartialOrder>);

	//值反链包装器
	template<auto PartialOrder = try_to_trust_existing_relation>
	struct antichain
	{
		template<auto... Value>
		struct wrapping
			: value_list<Value...>
			, value_antichain_tag
			, partial_order_induction<PartialOrder>
		{
			static constexpr auto partial_order{ PartialOrder };
		};

		template<auto... Value>
			requires AntichainWitness<value_list<Value...>, PartialOrder>
		using of = wrapping<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenAntichain<Range, PartialOrder>;
	};

	template<>
	struct antichain<try_to_trust_existing_relation>
	{
		template<auto... Value>
		using wrapping = antichain<default_partial_order>::wrapping<Value...>;

		template<auto... Value>
		using of = antichain<default_partial_order>::of<Value...>;

		template<typename Range>
		static constexpr bool proven = ProvenAntichain<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//值反链工厂
	template<auto... Value>
	using value_antichain = valuewise::antichain<>::of<Value...>;

}

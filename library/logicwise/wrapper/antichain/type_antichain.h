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
		::in<Range>()
		.satisfies(partial_order_induction<PartialOrder>::incomparability_relation);

	template<typename Range, auto PartialOrder = try_to_trust_existing_relation>
	concept Antichain =
		ProvenAntichain<Range, PartialOrder> ||
		(TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, default_partial_order>) ||
		(!TryToTrustExistingRelation<PartialOrder> && AntichainWitness<Range, PartialOrder>);

	//类型反链包装器
	template<auto PartialOrder = try_to_trust_existing_relation>
	struct antichain
	{
		template<typename... Type>
		struct wrapping
			: type_list<Type...>
			, type_antichain_tag
			, partial_order_induction<PartialOrder>
		{
			static constexpr auto partial_order{ PartialOrder };
		};

		template<typename... Type>
			requires AntichainWitness<type_list<Type...>, PartialOrder>
		using of = wrapping<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenAntichain<Range, PartialOrder>;
	};

	template<>
	struct antichain<try_to_trust_existing_relation>
	{
		template<typename... Type>
		using wrapping = antichain<default_partial_order>::wrapping<Type...>;

		template<typename... Type>
		using of = antichain<default_partial_order>::of<Type...>;

		template<typename Range>
		static constexpr bool proven = ProvenAntichain<Range>;
	};

}


//逻辑维度::细节
namespace logicwise::detail
{
	//类型反链工厂
	template<typename... Type>
	using type_antichain = typewise::antichain<>::of<Type...>;

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	struct type_list_error_reporter
	{
		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");
		}

		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate_with_index()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Index, typename Type>() -> bool { ... }");
		}

		template<typename EquivalenceRelation>
		static constexpr void incompatible_non_capturing_equivalence_relation()
		{
			static_assert(dependent_false_v<EquivalenceRelation>,
				"[logicwise] Error: Incompatible equivalence relation signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

		template<typename StrictWeakOrder>
		static constexpr void incompatible_non_capturing_strict_weak_order()
		{
			static_assert(dependent_false_v<StrictWeakOrder>,
				"[logicwise] Error: Incompatible strict weak order signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

		template<typename PartialOrder>
		static constexpr void incompatible_non_capturing_partial_order()
		{
			static_assert(dependent_false_v<PartialOrder>,
				"[logicwise] Error: Incompatible partial order signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

	};

}

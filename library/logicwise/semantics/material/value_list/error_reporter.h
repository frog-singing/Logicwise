// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	struct value_list_error_reporter
	{
		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Value>() -> bool { ... }");
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate_with_index()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Index, auto Value>() -> bool { ... }");
		}

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
		static constexpr void incompatible_non_capturing_equivalence_relation()
		{
			static_assert(dependent_false_v<EquivalenceRelation>,
				"[logicwise] Error: Incompatible equivalence relation signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename StrictWeakOrder>
		static constexpr void incompatible_non_capturing_strict_weak_order()
		{
			static_assert(dependent_false_v<StrictWeakOrder>,
				"[logicwise] Error: Incompatible strict weak order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename PartialWeakOrder>
		static constexpr void incompatible_non_capturing_partial_weak_order()
		{
			static_assert(dependent_false_v<PartialWeakOrder>,
				"[logicwise] Error: Incompatible partial weak order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename StrictPartialOrder>
		static constexpr void incompatible_non_capturing_strict_partial_order()
		{
			static_assert(dependent_false_v<StrictPartialOrder>,
				"[logicwise] Error: Incompatible strict partial order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename PartialPartialOrder>
		static constexpr void incompatible_non_capturing_partial_partial_order()
		{
			static_assert(dependent_false_v<PartialPartialOrder>,
				"[logicwise] Error: Incompatible partial partial order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

	};

}

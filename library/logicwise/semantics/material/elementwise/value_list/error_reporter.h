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
		template<auto Index>
		static constexpr void incompatible_index()
		{
			static_assert(valuewise_dependent_false_v<Index>,
				"[logicwise] Error: Incompatible index type!\n"
				"Expected: std::size_t");
		}

		//================================================================================

		template<typename PredicateType>
		static constexpr void incompatible_predicate()
		{
			static_assert(dependent_false_v<PredicateType>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected: [] <auto Value>() -> bool { ... }");
		}

		template<typename VerifierType>
		static constexpr void incompatible_verifier()
		{
			static_assert(dependent_false_v<VerifierType>,
				"[logicwise] Error: Incompatible verifier signature!\n"
				"Expected: [] <auto Value>() -> bool { ... }");
		}

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

		template<typename AcyclicRelation>
		static constexpr void incompatible_non_capturing_acyclic_relation()
		{
			static_assert(dependent_false_v<AcyclicRelation>,
				"[logicwise] Error: Incompatible acyclic relation signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

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

		//================================================================================

		template<typename OperationType>
		static constexpr void incompatible_operation()
		{
			static_assert(dependent_false_v<OperationType>,
				"[logicwise] Error: Incompatible operation signature!\n"
				"Expected: [&] <auto Value>() { ... }");
		}

		template<typename OperationType>
		static constexpr void incompatible_predicate_operation()
		{
			static_assert(dependent_false_v<OperationType>,
				"[logicwise] Error: Incompatible operation signature!\n"
				"Expected: [&] <auto Value>() -> bool { ... }");
		}

		//--------------------------------------------------------------------------------

		template<typename OperationType>
		static constexpr void incompatible_operation_with_index()
		{
			static_assert(dependent_false_v<OperationType>,
				"[logicwise] Error: Incompatible operation signature!\n"
				"Expected: [&] <auto Index, auto Value>() { ... }");
		}

		template<typename OperationType>
		static constexpr void incompatible_predicate_operation_with_index()
		{
			static_assert(dependent_false_v<OperationType>,
				"[logicwise] Error: Incompatible operation signature!\n"
				"Expected: [&] <auto Index, auto Value>() -> bool { ... }");
		}

		//================================================================================

		template<typename Combinator>
		static constexpr void incompatible_to_type_combinator()
		{
			static_assert(dependent_false_v<Combinator>,
				"[logicwise] Error: Incompatible combinator signature!\n"
				"Expected non-capturing lambda: [] <typename Accumulator, auto Value>()\n"
				"{ ... return std::type_identity<NextAccumulator>{}; }");
		}

		template<typename Combinator>
		static constexpr void incompatible_to_value_combinator()
		{
			static_assert(dependent_false_v<Combinator>,
				"[logicwise] Error: Incompatible combinator signature!\n"
				"Expected non-capturing lambda: [] <auto Accumulator, auto Value>()\n"
				"{ ... return NextAccumulator; }");
		}

		//--------------------------------------------------------------------------------

		template<typename Combinator>
		static constexpr void incompatible_to_type_combinator_with_index()
		{
			static_assert(dependent_false_v<Combinator>,
				"[logicwise] Error: Incompatible combinator signature!\n"
				"Expected non-capturing lambda: [] <auto Index, typename Accumulator, auto Value>()\n"
				"{ ... return std::type_identity<NextAccumulator>{}; }");
		}

		template<typename Combinator>
		static constexpr void incompatible_to_value_combinator_with_index()
		{
			static_assert(dependent_false_v<Combinator>,
				"[logicwise] Error: Incompatible combinator signature!\n"
				"Expected non-capturing lambda: [] <auto Index, auto Accumulator, auto Value>()\n"
				"{ ... return NextAccumulator; }");
		}

	};

}

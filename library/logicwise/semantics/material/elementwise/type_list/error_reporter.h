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
		template<auto Index>
		static constexpr void incompatible_index()
		{
			static_assert(valuewise_dependent_false_v<Index>,
				"[logicwise] Error: Incompatible index type!\n"
				"Expected: std::size_t");
		}

		template<typename InitialAccumulatorOrCombinator>
		static constexpr void unable_to_initiate_fold()
		{
			static_assert(dependent_false_v<InitialAccumulatorOrCombinator>,
				"[logicwise] Error: Unable to initiate fold!\n"
				"Check initial accumulator and combinator.");
		}

		struct elementwise
		{
			//谓词 predicate--------------------------------------------------------------------------------

			template<typename Predicate>
			static constexpr void incompatible_non_capturing_predicate()
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");
			}

			template<typename PredicateType>
			static constexpr void incompatible_predicate()
			{
				static_assert(dependent_false_v<PredicateType>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected: [] <typename Type>() -> bool { ... }");
			}

			template<typename VerifierType>
			static constexpr void incompatible_verifier()
			{
				static_assert(dependent_false_v<VerifierType>,
					"[logicwise] Error: Incompatible verifier signature!\n"
					"Expected: [] <typename Type>() -> bool { ... }");
			}

			//操作 operation--------------------------------------------------------------------------------
			
			template<typename OperationType>
			static constexpr void incompatible_operation()
			{
				static_assert(dependent_false_v<OperationType>,
					"[logicwise] Error: Incompatible operation signature!\n"
					"Expected: [&] <typename Type>() { ... }");
			}

			template<typename OperationType>
			static constexpr void incompatible_predicate_operation()
			{
				static_assert(dependent_false_v<OperationType>,
					"[logicwise] Error: Incompatible operation signature!\n"
					"Expected: [&] <typename Type>() -> bool { ... }");
			}

			//组合子 combinator--------------------------------------------------------------------------------

			template<typename Combinator>
			static constexpr void incompatible_to_type_combinator()
			{
				unable_to_initiate_fold<Combinator>();

				static_assert(dependent_false_v<Combinator>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected non-capturing lambda: [] <typename Accumulator, typename Type>()\n"
					"{ ... return std::type_identity<NextAccumulator>{}; }");
			}

			template<typename Combinator>
			static constexpr void incompatible_to_value_combinator()
			{
				unable_to_initiate_fold<Combinator>();

				static_assert(dependent_false_v<Combinator>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected non-capturing lambda: [] <auto Accumulator, typename Type>()\n"
					"{ ... return NextAccumulator; }");
			}

			template<typename CombinatorType>
			static constexpr void incompatible_to_instance_combinator()
			{
				unable_to_initiate_fold<CombinatorType>();

				static_assert(dependent_false_v<CombinatorType>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected: [] <typename Type>(auto&& accumulator)\n"
					"{ ... return next_accumulator; }");
			}

			//同质关系 homogeneous relation--------------------------------------------------------------------------------

			template<typename AcyclicRelation>
			static constexpr void incompatible_non_capturing_acyclic_relation()
			{
				static_assert(dependent_false_v<AcyclicRelation>,
					"[logicwise] Error: Incompatible acyclic relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
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

			template<typename PartialWeakOrder>
			static constexpr void incompatible_non_capturing_partial_weak_order()
			{
				static_assert(dependent_false_v<PartialWeakOrder>,
					"[logicwise] Error: Incompatible partial weak order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
			}

			template<typename StrictPartialOrder>
			static constexpr void incompatible_non_capturing_strict_partial_order()
			{
				static_assert(dependent_false_v<StrictPartialOrder>,
					"[logicwise] Error: Incompatible strict partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
			}

			template<typename PartialPartialOrder>
			static constexpr void incompatible_non_capturing_partial_partial_order()
			{
				static_assert(dependent_false_v<PartialPartialOrder>,
					"[logicwise] Error: Incompatible partial partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
			}

		};

		struct elementwise_with_index
		{
			//谓词 predicate--------------------------------------------------------------------------------

			template<typename Predicate>
			static constexpr void incompatible_non_capturing_predicate()
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <auto Index, typename Type>() -> bool { ... }");
			}

			//操作 operation--------------------------------------------------------------------------------

			template<typename OperationType>
			static constexpr void incompatible_operation()
			{
				static_assert(dependent_false_v<OperationType>,
					"[logicwise] Error: Incompatible operation signature!\n"
					"Expected: [&] <auto Index, typename Type>() { ... }");
			}

			template<typename OperationType>
			static constexpr void incompatible_predicate_operation()
			{
				static_assert(dependent_false_v<OperationType>,
					"[logicwise] Error: Incompatible operation signature!\n"
					"Expected: [&] <auto Index, typename Type>() -> bool { ... }");
			}

			//组合子 combinator--------------------------------------------------------------------------------

			template<typename Combinator>
			static constexpr void incompatible_to_type_combinator()
			{
				unable_to_initiate_fold<Combinator>();

				static_assert(dependent_false_v<Combinator>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected non-capturing lambda: [] <auto Index, typename Accumulator, typename Type>()\n"
					"{ ... return std::type_identity<NextAccumulator>{}; }");
			}

			template<typename Combinator>
			static constexpr void incompatible_to_value_combinator()
			{
				unable_to_initiate_fold<Combinator>();

				static_assert(dependent_false_v<Combinator>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected non-capturing lambda: [] <auto Index, auto Accumulator, typename Type>()\n"
					"{ ... return NextAccumulator; }");
			}

			template<typename CombinatorType>
			static constexpr void incompatible_to_instance_combinator()
			{
				unable_to_initiate_fold<CombinatorType>();

				static_assert(dependent_false_v<CombinatorType>,
					"[logicwise] Error: Incompatible combinator signature!\n"
					"Expected: [] <auto Index, typename Type>(auto&& accumulator)\n"
					"{ ... return next_accumulator; }");
			}

		};

	};

}

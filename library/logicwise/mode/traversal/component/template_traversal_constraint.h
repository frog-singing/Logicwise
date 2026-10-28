// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_traversal_constraint
	{
		using computation_operator = typename MaterialAgent::computation_operator;
		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto initial_index = extent_trait::probe_index;

		struct constraint
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			struct elementwise
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::elementwise::template instance_predicate_solver<PredicateType>
					::template solvable<initial_index>;

				template<typename OperationType>
				static constexpr bool valid_operation =
					computation_operator::elementwise::template operation_executor<OperationType>
					::template executable<initial_index>;

				template<typename InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_type_combinator =
					computation_operator::elementwise::template to_type_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<auto InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_value_combinator =
					computation_operator::elementwise::template to_value_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<typename InitialAccumulatorType, typename CombinatorType>
				static constexpr bool valid_to_instance_combinator =
					computation_operator::elementwise::template to_instance_combinator_folder<CombinatorType>
					::template foldable<InitialAccumulatorType, initial_index>;
							
			};

			struct elementwise_with_index
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::elementwise_with_index::template instance_predicate_solver<PredicateType>
					::template solvable<initial_index>;

				template<typename OperationType>
				static constexpr bool valid_operation =
					computation_operator::elementwise_with_index::template operation_executor<OperationType>
					::template executable<initial_index>;

				template<typename InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_type_combinator =
					computation_operator::elementwise_with_index::template to_type_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<auto InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_value_combinator =
					computation_operator::elementwise_with_index::template to_value_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<typename InitialAccumulatorType, typename CombinatorType>
				static constexpr bool valid_to_instance_combinator =
					computation_operator::elementwise_with_index::template to_instance_combinator_folder<CombinatorType>
					::template foldable<InitialAccumulatorType, initial_index>;

			};

			struct rangewise
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::rangewise::template instance_predicate_solver<PredicateType>
					::template solvable<initial_index>;

				template<typename OperationType>
				static constexpr bool valid_operation =
					computation_operator::rangewise::template operation_executor<OperationType>
					::template executable<initial_index>;

				template<typename InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_type_combinator =
					computation_operator::rangewise::template to_type_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<auto InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_value_combinator =
					computation_operator::rangewise::template to_value_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<typename InitialAccumulatorType, typename CombinatorType>
				static constexpr bool valid_to_instance_combinator =
					computation_operator::rangewise::template to_instance_combinator_folder<CombinatorType>
					::template foldable<InitialAccumulatorType, initial_index>;
							
			};

			struct rangewise_with_index
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::rangewise_with_index::template instance_predicate_solver<PredicateType>
					::template solvable<initial_index>;

				template<typename OperationType>
				static constexpr bool valid_operation =
					computation_operator::rangewise_with_index::template operation_executor<OperationType>
					::template executable<initial_index>;

				template<typename InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_type_combinator =
					computation_operator::rangewise_with_index::template to_type_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<auto InitialAccumulator, typename Combinator>
				static constexpr bool valid_to_value_combinator =
					computation_operator::rangewise_with_index::template to_value_combinator_folder<Combinator>
					::template foldable<InitialAccumulator, initial_index>;

				template<typename InitialAccumulatorType, typename CombinatorType>
				static constexpr bool valid_to_instance_combinator =
					computation_operator::rangewise_with_index::template to_instance_combinator_folder<CombinatorType>
					::template foldable<InitialAccumulatorType, initial_index>;

			};

		};

	};

}

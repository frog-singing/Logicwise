// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_traversal_constraint_component
	{
		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto initial_index = extent_trait::probe_index;

		struct component
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			template<typename PredicateType>
			static constexpr bool valid_predicate_type =
				MaterialAgent::template solvable_predicate_type<PredicateType, initial_index>;

			template<typename OperationType>
			static constexpr bool valid_operation_type =
				MaterialAgent::template executable_operation_type<OperationType, initial_index>;

			template<typename InitialAccumulator, typename Combinator>
			static constexpr bool valid_to_type_combinator =
				MaterialAgent::template to_type_combinator_folder<Combinator>
				::template foldable<InitialAccumulator, initial_index>;

			template<auto InitialAccumulator, typename Combinator>
			static constexpr bool valid_to_value_combinator =
				MaterialAgent::template to_value_combinator_folder<Combinator>
				::template foldable<InitialAccumulator, initial_index>;

		};

		struct component_with_index
		{
			template<typename PredicateType>
			static constexpr bool valid_predicate_with_index_type =
				MaterialAgent::template solvable_predicate_with_index_type<PredicateType, initial_index>;

			template<typename OperationType>
			static constexpr bool valid_operation_with_index_type =
				MaterialAgent::template executable_operation_with_index_type<OperationType, initial_index>;

			template<typename InitialAccumulator, typename Combinator>
			static constexpr bool valid_to_type_combinator_with_index =
				MaterialAgent::template to_type_combinator_with_index_folder<Combinator>
				::template foldable<InitialAccumulator, initial_index>;

			template<auto InitialAccumulator, typename Combinator>
			static constexpr bool valid_to_value_combinator_with_index =
				MaterialAgent::template to_value_combinator_with_index_folder<Combinator>
				::template foldable<InitialAccumulator, initial_index>;

		};

	};

}

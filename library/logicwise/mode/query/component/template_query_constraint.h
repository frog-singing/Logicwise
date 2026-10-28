// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_query_constraint
	{
		using index_type			= typename MaterialAgent::index_type;
		using computation_operator	= typename MaterialAgent::computation_operator;

		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto probe_index = extent_trait::probe_index;

		struct constraint
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			struct elementwise
			{
				template<typename Predicate>
				static constexpr bool valid_type_predicate =
					computation_operator::elementwise::template type_predicate_solver<Predicate>
					::template solvable<probe_index>;

				template<typename PredicateType>
				static constexpr bool valid_instance_predicate =
					computation_operator::elementwise::template instance_predicate_solver<PredicateType>
					::template solvable<probe_index>;

			};

			struct elementwise_with_index
			{
				template<auto Index>
				static constexpr bool index_with_valid_type = requires
				{
					index_type{ Index };
				};

			};

			struct rangewise
			{
				template<typename Predicate>
				static constexpr bool valid_type_predicate =
					computation_operator::rangewise::template type_predicate_solver<Predicate>
					::template solvable<probe_index>;

				template<typename PredicateType>
				static constexpr bool valid_instance_predicate =
					computation_operator::rangewise::template instance_predicate_solver<PredicateType>
					::template solvable<probe_index>;

			};

			//struct rangewise_with_index {};

		};

	};

}

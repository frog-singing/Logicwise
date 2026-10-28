// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_verification_constraint
	{
		using computation_operator = typename MaterialAgent::computation_operator;
		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto probe_index = extent_trait::probe_index;

		struct constraint
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			struct elementwise
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::elementwise::template instance_predicate_solver<PredicateType>
					::template solvable<probe_index>;

			};

			//struct elementwise_with_index {};

			struct rangewise
			{
				template<typename PredicateType>
				static constexpr bool valid_predicate =
					computation_operator::rangewise::template instance_predicate_solver<PredicateType>
					::template solvable<probe_index>;

			};

			//struct rangewise_with_index {};

		};

	};

}

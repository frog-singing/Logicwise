// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_query_constraint_component
	{
		using index_type = typename MaterialAgent::index_type;

		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto probe_index = extent_trait::probe_index;

		struct component
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			template<typename Predicate>
			static constexpr bool valid_predicate =
				MaterialAgent::template predicate_solver<Predicate>::template solvable<probe_index>;

			template<typename PredicateType>
			static constexpr bool valid_predicate_type =
				MaterialAgent::template solvable_predicate_type<PredicateType, probe_index>;

		};

		struct component_with_index
		{
			template<auto Index>
			static constexpr bool index_with_valid_type = requires
			{
				index_type{ Index };
			};

		};

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename MaterialAgent, typename Arrangement>
	struct template_verification_constraint_component
	{
		using extent_trait = typename MaterialAgent::template extent_trait<Arrangement>;
		static constexpr auto probe_index = extent_trait::probe_index;

		struct component
		{
			static constexpr bool traversable_extent = extent_trait::traversable;

			template<typename PredicateType>
			static constexpr bool valid_predicate_type =
				MaterialAgent::template solvable_predicate_type<PredicateType, probe_index>;

		};

		struct component_with_index
		{};

	};

}

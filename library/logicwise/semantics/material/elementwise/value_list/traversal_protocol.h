// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/element.h>
#include <logicwise/mode/traversal/executor/template_traversal_executor.h>
#include "agent.h"
#include "error_reporter.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 value_list 的实例
	template<typename List>
	struct value_list_traversal_protocol
	{
		using material_agent		= value_list_agent<List>;
		using traversal_executor	= template_traversal_executor<material_agent>;
		using error_reporter		= value_list_error_reporter;

		template<typename TraversalWorkshop>
		struct extended_core
		{
			using basic_core = typename TraversalWorkshop::basic_core;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::fold;
				using basic_core::fold_with_index;

				//================================================================================

				template<typename Accumulator, template<typename, auto> typename>
				[[nodiscard]] static constexpr auto fold() noexcept { return type_element<Accumulator>{}; }

				template<typename Accumulator, template<auto, typename, auto> typename>
				[[nodiscard]] static constexpr auto fold_with_index() noexcept { return type_element<Accumulator>{}; }

			};

		};

		template<typename TraversalWorkshop>
			requires TraversalWorkshop::constraint::traversable_extent
		struct extended_core<TraversalWorkshop>
		{
			template<template<typename Accumulator, auto Value> typename Combinator>
			using alias_to_type_combinator_folder =
				typename material_agent::template alias_to_type_combinator_folder<Combinator>;

			template<template<auto Index, typename Accumulator, auto Value> typename Combinator>
			using alias_to_type_combinator_with_index_folder =
				typename material_agent::template alias_to_type_combinator_with_index_folder<Combinator>;

			//--------------------------------------------------------------------------------

			using basic_core = typename TraversalWorkshop::basic_core;
			using extent_trait = typename TraversalWorkshop::extent_trait;

			template<typename Executor>
			static constexpr auto execute_traversal()
			{
				return TraversalWorkshop::template execute_traversal<Executor>();
			}

			//--------------------------------------------------------------------------------

			static constexpr auto initial_index = extent_trait::probe_index;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::fold;
				using basic_core::fold_with_index;

				//================================================================================

				template<
					typename InitialAccumulator,
					template<typename Accumulator, auto Value> typename ToTypeCombinator
				>
					requires alias_to_type_combinator_folder<ToTypeCombinator>
				::template foldable<InitialAccumulator, initial_index>
				[[nodiscard]] static constexpr auto fold()
				{
					return execute_traversal< typename traversal_executor::template fold_into_type<
						InitialAccumulator,
						alias_to_type_combinator_folder<ToTypeCombinator>
					> >();
				}

				template<
					typename InitialAccumulator,
					template<auto Index, typename Accumulator, auto Value> typename ToTypeCombinator
				>
					requires alias_to_type_combinator_with_index_folder<ToTypeCombinator>
				::template foldable<InitialAccumulator, initial_index>
				[[nodiscard]] static constexpr auto fold_with_index()
				{
					return execute_traversal< typename traversal_executor::template fold_into_type<
						InitialAccumulator,
						alias_to_type_combinator_with_index_folder<ToTypeCombinator>
					> >();
				}

				//================================================================================

				template<
					typename InitialAccumulator,
					template<typename Accumulator, auto Value> typename ToTypeCombinator
				>
				static constexpr auto fold()
				{
					error_reporter::template unable_to_initiate_fold<InitialAccumulator>();

					return type_element<InitialAccumulator>{};
				}

				template<
					typename InitialAccumulator,
					template<auto Index, typename Accumulator, auto Value> typename ToTypeCombinator
				>
				static constexpr auto fold_with_index()
				{
					error_reporter::template unable_to_initiate_fold<InitialAccumulator>();

					return type_element<InitialAccumulator>{};
				}

			};

		};

	};

}

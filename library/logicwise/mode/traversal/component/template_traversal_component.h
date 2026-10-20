// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/element.h>
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename TraversalWorkshop>
	struct template_traversal_component
	{
		struct component
		{
			static constexpr void execute(auto&&) noexcept {}
            static constexpr void execute_until(auto&&) noexcept {}
            static constexpr void execute_while(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto fold(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto fold(auto&&) noexcept { return Accumulator; }

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{
			static constexpr void execute_with_index(auto&&) noexcept {}
			static constexpr void execute_until_with_index(auto&&) noexcept {}
			static constexpr void execute_while_with_index(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto fold_with_index(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto fold_with_index(auto&&) noexcept { return Accumulator; }

		};

	};

	template<typename TraversalWorkshop>
		requires TraversalWorkshop::constraint::traversable_extent
	struct template_traversal_component<TraversalWorkshop>
	{
		using material_agent		= typename TraversalWorkshop::material_agent;
		using constraint			= typename TraversalWorkshop::constraint;
		using traversal_executor	= typename TraversalWorkshop::traversal_executor;
		using error_reporter		= typename TraversalWorkshop::error_reporter;

		template<typename Executor>
		static constexpr auto execute_traversal()
		{
			return TraversalWorkshop::template execute_traversal<Executor>();
		}

		template<typename Executor, typename OperationExecutorType>
		static constexpr auto execute_traversal(OperationExecutorType&& operation_executor)
		{
			return TraversalWorkshop::template execute_traversal<Executor>
				(std::forward<OperationExecutorType>(operation_executor));
		}

		//================================================================================

		struct component
		{
			template<typename OperationType>
				requires constraint::template valid_operation_type<OperationType>
			static constexpr void execute(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					([&] <auto Index> {
						material_agent::template execute_operation<Index>(operation);
					});
			}

			template<typename OperationType>
				requires constraint::template valid_predicate_type<OperationType>
			static constexpr void execute_until(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(operation);
					});
			}

			template<typename OperationType>
				requires constraint::template valid_predicate_type<OperationType>
			static constexpr void execute_while(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(operation);
					});
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::template valid_to_type_combinator<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto fold(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename material_agent::template to_type_combinator_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::template valid_to_value_combinator<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto fold(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename material_agent::template to_value_combinator_folder<ToValueCombinator>
				> >();
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void execute(OperationType&&)
			{
				error_reporter::template incompatible_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_until(OperationType&&)
			{
				error_reporter::template incompatible_predicate_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_while(OperationType&&)
			{
				error_reporter::template incompatible_predicate_operation<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto fold(ToTypeCombinator&&)
			{
				error_reporter::template incompatible_to_type_combinator<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto fold(ToValueCombinator&&)
			{
				error_reporter::template incompatible_to_value_combinator<ToValueCombinator>();

				return Accumulator;
			}

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{
			template<typename OperationType>
				requires constraint::template valid_operation_with_index_type<OperationType>
			static constexpr void execute_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					([&] <auto Index> {
						material_agent::template execute_operation_with_index<Index>(operation);
					});
			}

			template<typename OperationType>
				requires constraint::template valid_predicate_with_index_type<OperationType>
			static constexpr void execute_until_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					([&] <auto Index> { return
						material_agent::template solve_predicate_with_index<Index>(operation);
					});
			}

			template<typename OperationType>
				requires constraint::template valid_predicate_with_index_type<OperationType>
			static constexpr void execute_while_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					([&] <auto Index> { return
						material_agent::template solve_predicate_with_index<Index>(operation);
					});
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::template valid_to_type_combinator_with_index<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto fold_with_index(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename material_agent::template to_type_combinator_with_index_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::template valid_to_value_combinator_with_index<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto fold_with_index(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename material_agent::template to_value_combinator_with_index_folder<ToValueCombinator>
				> >();
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void execute_with_index(OperationType&&)
			{
				error_reporter::template incompatible_operation_with_index<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_until_with_index(OperationType&&)
			{
				error_reporter::template incompatible_predicate_operation_with_index<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_while_with_index(OperationType&&)
			{
				error_reporter::template incompatible_predicate_operation_with_index<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto fold_with_index(ToTypeCombinator&&)
			{
				error_reporter::template incompatible_to_type_combinator_with_index<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto fold_with_index(ToValueCombinator&&)
			{
				error_reporter::template incompatible_to_value_combinator_with_index<ToValueCombinator>();

				return Accumulator;
			}

		};

	};

}

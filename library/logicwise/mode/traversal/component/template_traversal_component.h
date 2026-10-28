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
		struct elementwise
		{
			static constexpr void execute(auto&&) noexcept {}
            static constexpr void execute_until(auto&&) noexcept {}
            static constexpr void execute_while(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto fold(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto fold(auto&&) noexcept { return Accumulator; }

			[[nodiscard]] static constexpr auto fold(auto&& accumulator, auto&&) noexcept { return accumulator; }

		};

		struct elementwise_with_index
		{
			static constexpr void execute_with_index(auto&&) noexcept {}
			static constexpr void execute_until_with_index(auto&&) noexcept {}
			static constexpr void execute_while_with_index(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto fold_with_index(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto fold_with_index(auto&&) noexcept { return Accumulator; }

			[[nodiscard]] static constexpr auto fold_with_index(auto&& accumulator, auto&&) noexcept { return accumulator; }

		};

		struct rangewise
		{
			static constexpr void rangewise_execute(auto&&) noexcept {}
			static constexpr void rangewise_execute_until(auto&&) noexcept {}
			static constexpr void rangewise_execute_while(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto rangewise_fold(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto rangewise_fold(auto&&) noexcept { return Accumulator; }

			[[nodiscard]] static constexpr auto rangewise_fold(auto&& accumulator, auto&&) noexcept { return accumulator; }

		};

		struct rangewise_with_index
		{
			static constexpr void rangewise_execute_with_index(auto&&) noexcept {}
			static constexpr void rangewise_execute_until_with_index(auto&&) noexcept {}
			static constexpr void rangewise_execute_while_with_index(auto&&) noexcept {}

			template<typename Accumulator>
			[[nodiscard]] static constexpr auto rangewise_fold_with_index(auto&&) noexcept { return type_element<Accumulator>{}; }

			template<auto Accumulator>
			[[nodiscard]] static constexpr auto rangewise_fold_with_index(auto&&) noexcept { return Accumulator; }

			[[nodiscard]] static constexpr auto rangewise_fold_with_index(auto&& accumulator, auto&&) noexcept { return accumulator; }

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

		template<typename Executor, typename... Argument>
		static constexpr auto execute_traversal(Argument&&... argument)
		{
			return TraversalWorkshop::template execute_traversal<Executor>(
				std::forward<Argument>(argument)...
			);
		}

		//--------------------------------------------------------------------------------

		using computation_operator = typename material_agent::computation_operator;

		//================================================================================

		struct elementwise
		{
			template<typename OperationType>
				requires constraint::elementwise::template valid_operation<OperationType>
			static constexpr void execute(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					(typename computation_operator::elementwise::operation_executor{ operation });
			}

			template<typename OperationType>
				requires constraint::elementwise::template valid_predicate<OperationType>
			static constexpr void execute_until(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					(typename computation_operator::elementwise::instance_predicate_solver{ operation });
			}

			template<typename OperationType>
				requires constraint::elementwise::template valid_predicate<OperationType>
			static constexpr void execute_while(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					(typename computation_operator::elementwise::instance_predicate_solver{ operation });
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::elementwise::template valid_to_type_combinator<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto fold(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename computation_operator::elementwise::template to_type_combinator_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::elementwise::template valid_to_value_combinator<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto fold(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename computation_operator::elementwise::template to_value_combinator_folder<ToValueCombinator>
				> >();
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
				requires constraint::elementwise::template valid_to_instance_combinator<AccumulatorType, ToInstanceCombinatorType>
			[[nodiscard]] static constexpr auto fold(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&& to_instance_combinator)
			{
				return execute_traversal< typename traversal_executor::fold_into_instance >(
					std::forward<AccumulatorType>(accumulator),
					typename computation_operator::elementwise::to_instance_combinator_folder{ to_instance_combinator }
				);
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void execute(OperationType&&)
			{
				error_reporter::elementwise::template incompatible_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_until(OperationType&&)
			{
				error_reporter::elementwise::template incompatible_predicate_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_while(OperationType&&)
			{
				error_reporter::elementwise::template incompatible_predicate_operation<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto fold(ToTypeCombinator&&)
			{
				error_reporter::elementwise::template incompatible_to_type_combinator<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto fold(ToValueCombinator&&)
			{
				error_reporter::elementwise::template incompatible_to_value_combinator<ToValueCombinator>();

				return Accumulator;
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
			static constexpr auto fold(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&&)
			{
				error_reporter::elementwise::template incompatible_to_instance_combinator<ToInstanceCombinatorType>();

				return accumulator;
			}

		};

		struct elementwise_with_index
		{
			template<typename OperationType>
				requires constraint::elementwise_with_index::template valid_operation<OperationType>
			static constexpr void execute_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					(typename computation_operator::elementwise_with_index::operation_executor{ operation });
			}

			template<typename OperationType>
				requires constraint::elementwise_with_index::template valid_predicate<OperationType>
			static constexpr void execute_until_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					(typename computation_operator::elementwise_with_index::instance_predicate_solver{ operation });
			}

			template<typename OperationType>
				requires constraint::elementwise_with_index::template valid_predicate<OperationType>
			static constexpr void execute_while_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					(typename computation_operator::elementwise_with_index::instance_predicate_solver{ operation });
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::elementwise_with_index::template valid_to_type_combinator<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto fold_with_index(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename computation_operator::elementwise_with_index::template to_type_combinator_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::elementwise_with_index::template valid_to_value_combinator<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto fold_with_index(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename computation_operator::elementwise_with_index::template to_value_combinator_folder<ToValueCombinator>
				> >();
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
				requires constraint::elementwise_with_index::template valid_to_instance_combinator<AccumulatorType, ToInstanceCombinatorType>
			[[nodiscard]] static constexpr auto fold_with_index(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&& to_instance_combinator)
			{
				return execute_traversal< typename traversal_executor::fold_into_instance >(
					std::forward<AccumulatorType>(accumulator),
					typename computation_operator::elementwise_with_index::to_instance_combinator_folder{ to_instance_combinator }
				);
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void execute_with_index(OperationType&&)
			{
				error_reporter::elementwise_with_index::template incompatible_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_until_with_index(OperationType&&)
			{
				error_reporter::elementwise_with_index::template incompatible_predicate_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void execute_while_with_index(OperationType&&)
			{
				error_reporter::elementwise_with_index::template incompatible_predicate_operation<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto fold_with_index(ToTypeCombinator&&)
			{
				error_reporter::elementwise_with_index::template incompatible_to_type_combinator<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto fold_with_index(ToValueCombinator&&)
			{
				error_reporter::elementwise_with_index::template incompatible_to_value_combinator<ToValueCombinator>();

				return Accumulator;
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
			static constexpr auto fold_with_index(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&&)
			{
				error_reporter::elementwise_with_index::template incompatible_to_instance_combinator<ToInstanceCombinatorType>();

				return accumulator;
			}

		};

		struct rangewise
		{
			template<typename OperationType>
				requires constraint::rangewise::template valid_operation<OperationType>
			static constexpr void rangewise_execute(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					(typename computation_operator::rangewise::operation_executor{ operation });
			}

			template<typename OperationType>
				requires constraint::rangewise::template valid_predicate<OperationType>
			static constexpr void rangewise_execute_until(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					(typename computation_operator::rangewise::instance_predicate_solver{ operation });
			}

			template<typename OperationType>
				requires constraint::rangewise::template valid_predicate<OperationType>
			static constexpr void rangewise_execute_while(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					(typename computation_operator::rangewise::instance_predicate_solver{ operation });
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::rangewise::template valid_to_type_combinator<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto rangewise_fold(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename computation_operator::rangewise::template to_type_combinator_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::rangewise::template valid_to_value_combinator<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto rangewise_fold(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename computation_operator::rangewise::template to_value_combinator_folder<ToValueCombinator>
				> >();
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
				requires constraint::rangewise::template valid_to_instance_combinator<AccumulatorType, ToInstanceCombinatorType>
			[[nodiscard]] static constexpr auto rangewise_fold(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&& to_instance_combinator)
			{
				return execute_traversal< typename traversal_executor::fold_into_instance >(
					std::forward<AccumulatorType>(accumulator),
					typename computation_operator::rangewise::to_instance_combinator_folder{ to_instance_combinator }
				);
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void rangewise_execute(OperationType&&)
			{
				error_reporter::rangewise::template incompatible_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void rangewise_execute_until(OperationType&&)
			{
				error_reporter::rangewise::template incompatible_predicate_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void rangewise_execute_while(OperationType&&)
			{
				error_reporter::rangewise::template incompatible_predicate_operation<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto rangewise_fold(ToTypeCombinator&&)
			{
				error_reporter::rangewise::template incompatible_to_type_combinator<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto rangewise_fold(ToValueCombinator&&)
			{
				error_reporter::rangewise::template incompatible_to_value_combinator<ToValueCombinator>();

				return Accumulator;
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
			static constexpr auto rangewise_fold(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&&)
			{
				error_reporter::rangewise::template incompatible_to_instance_combinator<ToInstanceCombinatorType>();

				return accumulator;
			}

		};

		struct rangewise_with_index
		{
			template<typename OperationType>
				requires constraint::rangewise_with_index::template valid_operation<OperationType>
			static constexpr void rangewise_execute_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute >
					(typename computation_operator::rangewise_with_index::operation_executor{ operation });
			}

			template<typename OperationType>
				requires constraint::rangewise_with_index::template valid_predicate<OperationType>
			static constexpr void rangewise_execute_until_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_until >
					(typename computation_operator::rangewise_with_index::instance_predicate_solver{ operation });
			}

			template<typename OperationType>
				requires constraint::rangewise_with_index::template valid_predicate<OperationType>
			static constexpr void rangewise_execute_while_with_index(OperationType&& operation)
			{
				execute_traversal< typename traversal_executor::execute_while >
					(typename computation_operator::rangewise_with_index::instance_predicate_solver{ operation });
			}

			template<typename Accumulator, typename ToTypeCombinator>
				requires constraint::rangewise_with_index::template valid_to_type_combinator<Accumulator, ToTypeCombinator>
			[[nodiscard]] static constexpr auto rangewise_fold_with_index(ToTypeCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_type<
					Accumulator,
					typename computation_operator::rangewise_with_index::template to_type_combinator_folder<ToTypeCombinator>
				> >();
			}

			template<auto Accumulator, typename ToValueCombinator>
				requires constraint::rangewise_with_index::template valid_to_value_combinator<Accumulator, ToValueCombinator>
			[[nodiscard]] static constexpr auto rangewise_fold_with_index(ToValueCombinator&&)
			{
				return execute_traversal< typename traversal_executor::template fold_into_value<
					Accumulator,
					typename computation_operator::rangewise_with_index::template to_value_combinator_folder<ToValueCombinator>
				> >();
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
				requires constraint::rangewise_with_index::template valid_to_instance_combinator<AccumulatorType, ToInstanceCombinatorType>
			[[nodiscard]] static constexpr auto rangewise_fold_with_index(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&& to_instance_combinator)
			{
				return execute_traversal< typename traversal_executor::fold_into_instance >(
					std::forward<AccumulatorType>(accumulator),
					typename computation_operator::rangewise_with_index::to_instance_combinator_folder{ to_instance_combinator }
				);
			}

			//================================================================================

			template<typename OperationType>
			static constexpr void rangewise_execute_with_index(OperationType&&)
			{
				error_reporter::rangewise_with_index::template incompatible_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void rangewise_execute_until_with_index(OperationType&&)
			{
				error_reporter::rangewise_with_index::template incompatible_predicate_operation<OperationType>();
			}

			template<typename OperationType>
			static constexpr void rangewise_execute_while_with_index(OperationType&&)
			{
				error_reporter::rangewise_with_index::template incompatible_predicate_operation<OperationType>();
			}

			template<typename Accumulator, typename ToTypeCombinator>
			static constexpr auto rangewise_fold_with_index(ToTypeCombinator&&)
			{
				error_reporter::rangewise_with_index::template incompatible_to_type_combinator<ToTypeCombinator>();

				return type_element<Accumulator>{};
			}

			template<auto Accumulator, typename ToValueCombinator>
			static constexpr auto rangewise_fold_with_index(ToValueCombinator&&)
			{
				error_reporter::rangewise_with_index::template incompatible_to_value_combinator<ToValueCombinator>();

				return Accumulator;
			}

			template<typename AccumulatorType, typename ToInstanceCombinatorType>
			static constexpr auto rangewise_fold_with_index(
				AccumulatorType&& accumulator, ToInstanceCombinatorType&&)
			{
				error_reporter::rangewise_with_index::template incompatible_to_instance_combinator<ToInstanceCombinatorType>();

				return accumulator;
			}

		};

	};

}

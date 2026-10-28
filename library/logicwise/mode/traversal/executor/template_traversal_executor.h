// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/element.h>
#include <logicwise/index/index_sequencer.h>
#include "traversal_loop.h"
#include <utility> //用于 std::forward, std::move


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename MaterialAgent>
	struct template_traversal_executor
	{
		using index_type = typename MaterialAgent::index_type;
		static constexpr auto extent = MaterialAgent::extent;

		//================================================================================

		//规避类名和方法名不能同名的语法限制
		struct execute_impl
		{
			template<typename Arrangement, typename IndexTraverserType, typename OperationExecutorType>
			static constexpr void execute(OperationExecutorType&& operation_executor)
			{
				template_execute_loop<Arrangement, IndexTraverserType, extent>
					(std::forward<OperationExecutorType>(operation_executor));
			}
		};

		using execute = execute_impl;

		struct execute_until
		{
			template<typename Arrangement, typename IndexTraverserType, typename PredicateSolverType>
			static constexpr void execute(PredicateSolverType&& predicate_solver)
			{
				template_execute_until_loop<Arrangement, IndexTraverserType, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

		struct execute_while
		{
			template<typename Arrangement, typename IndexTraverserType, typename PredicateSolverType>
			static constexpr void execute(PredicateSolverType&& predicate_solver)
			{
				template_execute_while_loop<Arrangement, IndexTraverserType, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

		template<typename InitialAccumulator, typename CombinatorFolder>
		struct fold_into_type
		{
			template<typename Accumulator>
			struct accumulator_proxy
			{
				using accumulator = Accumulator;

				template<typename IndexWrapper>
				constexpr auto operator+(IndexWrapper) const
				{
					return accumulator_proxy<
						typename CombinatorFolder::template fold<Accumulator, IndexWrapper::value>
					>{};
				}
			};

			template<index_type... Index>
			using fold = typename decltype(
				(accumulator_proxy<InitialAccumulator>{} + ... + value_element<Index>{})
			)::accumulator;

			template<typename Arrangement, typename IndexTraverserType>
			static constexpr auto execute()
			{
				using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, extent>;
				using index_sequence = typename IndexSequencer::index_sequence;

				return type_element< typename index_sequence::template apply<fold> >{};
			}
		};

		template<auto InitialAccumulator, typename CombinatorFolder>
		struct fold_into_value
		{
			template<auto Accumulator>
			struct accumulator_proxy
			{
				static constexpr auto accumulator{ Accumulator };

				template<typename IndexWrapper>
				constexpr auto operator+(IndexWrapper) const
				{
					return accumulator_proxy<
						CombinatorFolder::template fold<Accumulator, IndexWrapper::value>()
					>{};
				}
			};

			static constexpr auto fold = [] <index_type... Index> {
				return (accumulator_proxy<InitialAccumulator>{} + ... + value_element<Index>{}).accumulator;
			};

			template<typename Arrangement, typename IndexTraverserType>
			static constexpr auto execute()
			{
				using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, extent>;
				using index_sequence = typename IndexSequencer::index_sequence;
				
				return index_sequence::invoke(fold);
			}
		};

		struct fold_into_instance
		{
			template<typename AccumulatorType, typename CombinatorFolderType>
			struct fold
			{
				AccumulatorType accumulator;
				CombinatorFolderType combinator_folder;

				template<typename IndexWrapper>
				constexpr auto operator+(IndexWrapper)
				{
					auto next_accumulator = combinator_folder.template fold<IndexWrapper::value>(std::move(accumulator));

					return fold<decltype(next_accumulator), CombinatorFolderType>{
						std::move(next_accumulator),
						combinator_folder
					};
				}

				template<index_type... Index>
				constexpr auto operator()()
				{
					return (*this + ... + value_element<Index>{}).accumulator;
				}
			};

			template<typename Arrangement, typename IndexTraverserType, typename InitialAccumulatorType, typename CombinatorFolderType>
			static constexpr auto execute(InitialAccumulatorType&& initial_accumulator, CombinatorFolderType combinator_folder)
			{
				using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, extent>;
				using index_sequence = typename IndexSequencer::index_sequence;

				return index_sequence::invoke(
					fold{ std::forward<InitialAccumulatorType>(initial_accumulator), combinator_folder }
				);
			}
		};

	};

}

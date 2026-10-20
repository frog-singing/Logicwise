// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/element.h>
#include <logicwise/index/index_sequencer.h>
#include "traversal_loop.h"
#include <utility> //用于 std::forward


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
			template<typename Arrangement, typename IndexTraverserType, typename OperationExecutorType>
			static constexpr void execute(OperationExecutorType&& operation_executor)
			{
				template_execute_until_loop<Arrangement, IndexTraverserType, extent>
					(std::forward<OperationExecutorType>(operation_executor));
			}
		};

		struct execute_while
		{
			template<typename Arrangement, typename IndexTraverserType, typename OperationExecutorType>
			static constexpr void execute(OperationExecutorType&& operation_executor)
			{
				template_execute_while_loop<Arrangement, IndexTraverserType, extent>
					(std::forward<OperationExecutorType>(operation_executor));
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
				return decltype(
					(accumulator_proxy<InitialAccumulator>{} + ... + value_element<Index>{})
				)::accumulator;
			};

			template<typename Arrangement, typename IndexTraverserType>
			static constexpr auto execute()
			{
				using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, extent>;
				using index_sequence = typename IndexSequencer::index_sequence;
				
				return index_sequence::invoke(fold);
			}
		};

	};

}

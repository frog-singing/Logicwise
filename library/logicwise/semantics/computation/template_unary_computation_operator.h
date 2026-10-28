// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::计算 semantics::computation================================================================================

	template<typename ComputationPrimitive>
	struct template_unary_computation_operator
	{
		//谓词 predicate================================================================================

		template<typename Predicate>
		struct type_predicate_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires
			{
				bool{ ComputationPrimitive::template invoke_at<Predicate, Index>() };
			};

			template<auto Index>
			static constexpr bool solve()
			{
				return ComputationPrimitive::template invoke_at<Predicate, Index>();
			}

			template<auto Index>
			constexpr bool operator()()
			{
				return solve<Index>();
			}
		};

		//--------------------------------------------------------------------------------

		template<typename PredicateType>
		struct instance_predicate_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires(PredicateType & predicate)
			{
				bool{ ComputationPrimitive::template invoke_at<Index>(predicate) };
			};

			PredicateType& predicate;

			template<auto Index>
			constexpr bool solve()
			{
				return ComputationPrimitive::template invoke_at<Index>(predicate);
			}

			template<auto Index>
			constexpr bool operator()()
			{
				return solve<Index>();
			}
		};

		//操作 operation================================================================================

		template<typename OperationType>
		struct operation_executor
		{
			template<auto Index>
			static constexpr bool executable = requires(OperationType & operation)
			{
				ComputationPrimitive::template invoke_at<Index>(operation);
			};

			OperationType& operation;

			template<auto Index>
			constexpr void execute()
			{
				ComputationPrimitive::template invoke_at<Index>(operation);
			}

			template<auto Index>
			constexpr void operator()()
			{
				execute<Index>();
			}

		};

		//组合子 combinator================================================================================

		template<typename Combinator>
		struct to_type_combinator_folder
		{
			template<typename Accumulator, auto Index>
			static constexpr bool foldable = requires
			{
				typename ComputationPrimitive::template combine_type_at<Combinator, Accumulator, Index>;
			};

			template<typename Accumulator, auto Index>
			using fold = typename ComputationPrimitive::template combine_type_at<Combinator, Accumulator, Index>;
		};

		//--------------------------------------------------------------------------------

		template<typename Combinator>
		struct to_value_combinator_folder
		{
			template<auto Accumulator, auto Index>
			static constexpr bool foldable = requires
			{
				ComputationPrimitive::template combine_value_at<Combinator, Accumulator, Index>();
			};

			template<auto Accumulator, auto Index>
			static constexpr auto fold()
			{
				return ComputationPrimitive::template combine_value_at<Combinator, Accumulator, Index>();
			}
		};

		//--------------------------------------------------------------------------------

		template<typename CombinatorType>
		struct to_instance_combinator_folder
		{
			template<typename AccumulatorType, auto Index>
			static constexpr bool foldable =
				requires(CombinatorType & combinator, AccumulatorType && accumulator)
			{
				ComputationPrimitive::template combine_instance_at<Index>(
					combinator,
					std::forward<AccumulatorType>(accumulator)
				);
			};

			CombinatorType& combinator;

			template<auto Index, typename AccumulatorType>
			constexpr auto fold(AccumulatorType&& accumulator)
			{
				return ComputationPrimitive::template combine_instance_at<Index>(
					combinator,
					std::forward<AccumulatorType>(accumulator)
				);
			}
		};

	};

}

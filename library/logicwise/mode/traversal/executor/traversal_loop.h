// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>

#include <logicwise/index/index_sequencer.h>
#include <functional> //用于 std::invoke


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename OperationExecutorType>
	constexpr void template_execute_loop(OperationExecutorType&& operation_executor)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			operation_executor.template operator() < Index > ();
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(..., operation_executor.template operator() < Index > ());
		});
#endif
	}

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename PredicateSolverType>
	constexpr void template_execute_until_loop(PredicateSolverType&& predicate_solver)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			if (predicate_solver.template operator() < Index > ()) { return; }
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(... || predicate_solver.template operator() < Index > ());
		});
#endif
	}

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename PredicateSolverType>
	constexpr void template_execute_while_loop(PredicateSolverType&& predicate_solver)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			if (!predicate_solver.template operator() < Index > ()) { return; }
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(... && predicate_solver.template operator() < Index > ());
		});
#endif
	}
	
	template<typename Arrangement, typename IndexTraverserType, typename OperationExecutorType>
	constexpr void instance_execute_loop(typename Arrangement::extent_type extent,
		OperationExecutorType&& operation_executor)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			std::invoke(operation_executor, index_traverser.state());
			index_traverser.step();
		}
	}

	template<typename Arrangement, typename IndexTraverserType, typename PredicateSolverType>
	constexpr void instance_execute_until_loop(typename Arrangement::extent_type extent,
		PredicateSolverType&& predicate_solver)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			if (std::invoke(predicate_solver, index_traverser.state())) { return; }
			index_traverser.step();
		}
	}

	template<typename Arrangement, typename IndexTraverserType, typename PredicateSolverType>
	constexpr void instance_execute_while_loop(typename Arrangement::extent_type extent,
		PredicateSolverType&& predicate_solver)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			if (!std::invoke(predicate_solver, index_traverser.state())) { return; }
			index_traverser.step();
		}
	}

}

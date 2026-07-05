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
	//行为模式::遍历 mode::traversal================================================================================

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename AtomicOperationType>
	constexpr void template_execute_loop(AtomicOperationType&& atomic_operation)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			atomic_operation.template operator() < Index > ();
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(..., atomic_operation.template operator() < Index > ());
		});
#endif
	}

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename AtomicOperationType>
	constexpr void template_execute_until_loop(AtomicOperationType&& atomic_operation)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			if (atomic_operation.template operator() < Index > ()) { return; }
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(... || atomic_operation.template operator() < Index > ());
		});
#endif
	}

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent,
		typename AtomicOperationType>
	constexpr void template_execute_while_loop(AtomicOperationType&& atomic_operation)
	{
		using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = IndexSequencer::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			if (!atomic_operation.template operator() < Index > ()) { return; }
		}
#else
		//C++20
		using index_sequence = typename IndexSequencer::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(... && atomic_operation.template operator() < Index > ());
		});
#endif
	}
	
	template<typename Arrangement, typename IndexTraverserType, typename AtomicOperationType>
	constexpr void instance_execute_loop(typename Arrangement::extent_type extent,
		AtomicOperationType&& atomic_operation)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			std::invoke(atomic_operation, index_traverser.state());
			index_traverser.step();
		}
	}

	template<typename Arrangement, typename IndexTraverserType, typename AtomicOperationType>
	constexpr void instance_execute_until_loop(typename Arrangement::extent_type extent,
		AtomicOperationType&& atomic_operation)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			if (std::invoke(atomic_operation, index_traverser.state())) { return; }
			index_traverser.step();
		}
	}

	template<typename Arrangement, typename IndexTraverserType, typename AtomicOperationType>
	constexpr void instance_execute_while_loop(typename Arrangement::extent_type extent,
		AtomicOperationType&& atomic_operation)
	{
		IndexTraverserType index_traverser{ extent };

		while (!index_traverser.done())
		{
			if (!std::invoke(atomic_operation, index_traverser.state())) { return; }
			index_traverser.step();
		}
	}

}

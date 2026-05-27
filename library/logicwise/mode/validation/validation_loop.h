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
	//行为模式::验证 mode::validation================================================================================

	template<typename Quantifier, typename Arrangement, typename Arrangement::extent_type Extent,
		typename AtomicValidationType>
	static constexpr bool template_validation_loop(AtomicValidationType&& atomic_validation)
	{
		using index_traverser_type = typename Arrangement::fast_index_traverser;
		using index_sequencer_instance = index_sequencer<Arrangement, index_traverser_type, Extent>;

		typename Quantifier::solver quantifier_solver{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
		//C++26
		constexpr auto index_array = index_sequencer_instance::generate_index_array();

		template for (constexpr auto Index : index_array)
		{
			if (quantifier_solver.solved()) { break; }

			quantifier_solver.step(atomic_validation.template operator() < Index > ());
		}
#else
		//C++20
		using index_sequence = typename index_sequencer_instance::index_sequence;

		index_sequence::invoke([&] <auto... Index> {
			(..., (
				quantifier_solver.solved() ? void() :
				quantifier_solver.step(atomic_validation.template operator() < Index > ())
			));																								
		});
#endif

		return quantifier_solver.result();
	}

	template<typename Quantifier, typename Arrangement, typename AtomicValidationType>
	static constexpr bool instance_validation_loop(typename Arrangement::extent_type extent,
		AtomicValidationType&& atomic_validation)
	{
		using index_traverser_type = typename Arrangement::fast_index_traverser;

		typename Quantifier::solver quantifier_solver{};
		index_traverser_type index_traverser{ extent };

		while (!quantifier_solver.solved() && !index_traverser.done())
		{
			quantifier_solver.step(std::invoke(atomic_validation, index_traverser.state()));
			index_traverser.step();
		}

		return quantifier_solver.result();
	}

}

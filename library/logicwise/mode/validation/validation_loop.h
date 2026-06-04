// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/traversal/traversal_loop.h>
#include <functional> //用于 std::invoke


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

	template<typename Quantifier, typename Arrangement, typename Arrangement::extent_type Extent,
		typename AtomicValidationType>
	static constexpr bool template_validation_loop(AtomicValidationType&& atomic_validation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		typename Quantifier::solver quantifier_solver{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (quantifier_solver.solved()) { return true; }
				quantifier_solver.step(atomic_validation.template operator() < Index > ());
				return false;
			});

		return quantifier_solver.result();
	}

	template<typename Quantifier, typename Arrangement, typename AtomicValidationType>
	static constexpr bool instance_validation_loop(typename Arrangement::extent_type extent,
		AtomicValidationType&& atomic_validation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		typename Quantifier::solver quantifier_solver{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto index) {
				if (quantifier_solver.solved()) { return true; }
				quantifier_solver.step(std::invoke(atomic_validation, index));
				return false;
			});

		return quantifier_solver.result();
	}

}

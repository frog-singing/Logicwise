// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/traversal/executor/traversal_loop.h>
#include <functional> //用于 std::invoke


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename Quantifier, typename Arrangement, typename Arrangement::extent_type Extent,
		typename PredicateSolverType>
	constexpr bool template_satisfies_loop(PredicateSolverType&& predicate_solver)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		typename Quantifier::solver quantifier_solver{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (quantifier_solver.solved()) { return true; }
				quantifier_solver.step(predicate_solver.template operator() < Index > ());
				return false;
			});

		return quantifier_solver.result();
	}

	template<typename Quantifier, typename Arrangement, typename PredicateSolverType>
	constexpr bool instance_satisfies_loop(typename Arrangement::extent_type extent,
		PredicateSolverType&& predicate_solver)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		typename Quantifier::solver quantifier_solver{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (quantifier_solver.solved()) { return true; }
				quantifier_solver.step(std::invoke(predicate_solver, index));
				return false;
			});

		return quantifier_solver.result();
	}

}

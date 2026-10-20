// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "verification_loop.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename MaterialAgent>
	struct template_verification_executor
	{
		static constexpr auto extent = MaterialAgent::extent;

		//================================================================================

		struct satisfies
		{
			template<typename Quantifier, typename Arrangement, typename PredicateSolverType>
			static constexpr bool execute(PredicateSolverType&& predicate_solver)
			{
				return template_satisfies_loop<Quantifier, Arrangement, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

	};

}

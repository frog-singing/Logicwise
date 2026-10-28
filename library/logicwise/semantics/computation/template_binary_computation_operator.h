// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::计算 semantics::computation================================================================================

	template<typename ComputationPrimitive>
	struct template_binary_computation_operator
	{
		//同质关系 homogeneous relation================================================================================

		template<typename HomogeneousRelation>
		struct type_homogeneous_relation_solver
		{
			template<auto Index1, auto Index2>
			static constexpr bool solvable = requires
			{
				bool{ ComputationPrimitive::template invoke_at<HomogeneousRelation, Index1, Index2>() };
			};

			template<auto Index1, auto Index2>
			static constexpr bool solve()
			{
				return ComputationPrimitive::template invoke_at<HomogeneousRelation, Index1, Index2>();
			}

			template<auto Index1, auto Index2>
			constexpr bool operator()()
			{
				return solve<Index1, Index2>();
			}
		};

	};

}

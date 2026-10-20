// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	template<typename MaterialAgent, auto Data>
	struct template_view_constraint_component
	{
		//仅写为 || solvable<Data[...]> 无法将越界硬错误短路掉
		//写为 || requires { requires solvable<Data[...]> } 才能正确求值
		struct component
		{
			template<typename Predicate>
			static constexpr bool valid_predicate_for_first_element =
				(Data.size == 0) || requires
			{
				requires MaterialAgent::template predicate_solver<Predicate>::template solvable<Data[0]>;
			};

			template<typename Predicate>
			static constexpr bool valid_predicate_for_last_element =
				(Data.size == 0) || requires
			{
				requires MaterialAgent::template predicate_solver<Predicate>::template solvable<Data[Data.size - 1]>;
			};

			template<typename HomogeneousRelation>
			static constexpr bool valid_homogeneous_relation =
				(Data.size == 0) || requires
			{
				requires MaterialAgent::template homogeneous_relation_solver<HomogeneousRelation>::template solvable<Data[0], Data[0]>;
			};

		};

		struct component_with_index
		{
			template<typename Predicate>
			static constexpr bool valid_predicate_with_index =
				(Data.size == 0) || requires
			{
				requires MaterialAgent::template predicate_with_index_solver<Predicate>::template solvable<Data[0]>;
			};

		};

	};

}

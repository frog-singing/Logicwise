// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/index/rank.h>
#include "query_loop.h"
#include <cstddef> //用于 std::size_t
#include <utility> //用于 std::forward
#include <optional> //用于 std::optional


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename MaterialAgent>
	struct template_query_executor
	{
		using index_type = typename MaterialAgent::index_type;
		static constexpr auto extent = MaterialAgent::extent;

		//================================================================================

		struct count
		{
			template<typename Arrangement, typename PredicateSolverType>
			static constexpr std::size_t execute(PredicateSolverType&& predicate_solver)
			{
				return template_count_loop<Arrangement, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

		struct find_first_index
		{
			template<typename Arrangement, typename PredicateSolverType>
			static constexpr std::optional<index_type> execute(PredicateSolverType&& predicate_solver)
			{
				return template_find_first_index_loop<Arrangement, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

		struct find_last_index
		{
			template<typename Arrangement, typename PredicateSolverType>
			static constexpr std::optional<index_type> execute(PredicateSolverType&& predicate_solver)
			{
				return template_find_last_index_loop<Arrangement, extent>
					(std::forward<PredicateSolverType>(predicate_solver));
			}
		};

		//================================================================================

		template<typename DefaultContent, typename PredicateSolver>
		struct find_first
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				constexpr auto found_index = template_find_first_index_loop<Arrangement, extent>
					([] <auto Index> { return
						PredicateSolver::template solve<Index>();
					});

				if constexpr (found_index)
				{
					return MaterialAgent::template lookup<*found_index>();
				}
				else { return DefaultContent::null_object; }
			}
		};

		template<typename DefaultContent, typename PredicateSolver>
		struct find_last
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				constexpr auto found_index = template_find_last_index_loop<Arrangement, extent>
					([] <auto Index> { return
						PredicateSolver::template solve<Index>();
					});

				if constexpr (found_index)
				{
					return MaterialAgent::template lookup<*found_index>();
				}
				else { return DefaultContent::null_object; }
			}
		};

		//================================================================================

		template<std::size_t ScalarIndex>
		struct get
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				return MaterialAgent::template lookup<
					unrank<Arrangement, extent, ScalarIndex>()
				>();
			}
		};

		template<std::size_t ScalarIndex>
		struct at
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				static_assert(ScalarIndex < Arrangement::index_count(extent), "[logicwise] Scalar index out of range.");

				return MaterialAgent::template lookup<
					unrank<Arrangement, extent, ScalarIndex>()
				>();
			}
		};

		//================================================================================

		template<std::size_t ScalarIndex>
		struct index_at
		{
			template<typename Arrangement>
			static constexpr index_type execute()
			{
				static_assert(ScalarIndex < Arrangement::index_count(extent), "[logicwise] Scalar index out of range.");

				return unrank<Arrangement, extent, ScalarIndex>();
			}
		};

		template<index_type Index>
		struct lookup
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				return MaterialAgent::template lookup<Index>();
			}
		};

		template<index_type Index>
		struct try_lookup
		{
			template<typename Arrangement>
			static constexpr auto execute()
			{
				static_assert(Arrangement::valid_index(extent, Index), "[logicwise] Index out of range.");

				return MaterialAgent::template lookup<Index>();
			}
		};

	};

}

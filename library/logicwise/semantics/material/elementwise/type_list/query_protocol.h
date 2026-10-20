// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/query/executor/template_query_executor.h>
#include "agent.h"
#include "default_content.h"
#include "error_reporter.h"
#include <cstddef> //用于 std::size_t
#include <utility> //用于 std::forward
#include <optional> //用于 std::optional


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 type_list 的实例
	template<typename List>
	struct type_list_query_protocol
	{
		using material_agent	= type_list_agent<List>;
		using query_executor	= template_query_executor<material_agent>;
		using default_content	= type_list_default_content;
		using error_reporter	= type_list_error_reporter;

		template<typename QueryWorkshop>
		struct extended_core
		{
			using index_type = typename material_agent::index_type;
			static constexpr auto null_object = default_content::null_object;

			//--------------------------------------------------------------------------------

			using basic_core = typename QueryWorkshop::basic_core;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::count;
				using basic_core::find_first_index;
				using basic_core::find_last_index;
				using basic_core::find_first;
				using basic_core::find_last;

				//================================================================================

				template<template<typename> typename>
				[[nodiscard]] static constexpr std::size_t count() noexcept { return 0; }

				template<template<typename> typename>
				[[nodiscard]] static constexpr std::optional<index_type> find_first_index() noexcept { return {}; }

				template<template<typename> typename>
				[[nodiscard]] static constexpr std::optional<index_type> find_last_index() noexcept { return {}; }

				template<template<typename> typename>
				[[nodiscard]] static constexpr auto find_first() noexcept { return null_object; }

				template<template<typename> typename>
				[[nodiscard]] static constexpr auto find_last() noexcept { return null_object; }

			};

		};

		template<typename QueryWorkshop>
			requires QueryWorkshop::constraint::traversable_extent
		struct extended_core<QueryWorkshop>
		{
			using index_type = typename material_agent::index_type;

			template<template<typename> typename Predicate, index_type ProbeIndex>
			using trait_predicate_solver = typename material_agent::template trait_predicate_solver<Predicate, ProbeIndex>;
			
			//--------------------------------------------------------------------------------

			using basic_core	= typename QueryWorkshop::basic_core;
			using extent_trait	= typename QueryWorkshop::extent_trait;

			template<typename Executor>
			static constexpr auto execute_query()
			{
				return QueryWorkshop::template execute_query<Executor>();
			}

			template<typename Executor, typename PredicateSolverType>
			static constexpr auto execute_query(PredicateSolverType&& predicate_solver)
			{
				return QueryWorkshop::template execute_query<Executor>(std::forward<PredicateSolverType>(predicate_solver));
			}

			//--------------------------------------------------------------------------------

			static constexpr auto probe_index = extent_trait::probe_index;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::count;
				using basic_core::find_first_index;
				using basic_core::find_last_index;
				using basic_core::find_first;
				using basic_core::find_last;

				//================================================================================

				template<template<typename> typename Predicate>
					requires material_agent::template solvable_trait_predicate<Predicate, probe_index>
				[[nodiscard]] static constexpr std::size_t count()
				{
					return execute_query< typename query_executor::count >
						([] <auto Index> { return
							trait_predicate_solver<Predicate, probe_index>::template solve<Index>();
						});
				}

				template<template<typename> typename Predicate>
					requires material_agent::template solvable_trait_predicate<Predicate, probe_index>
				[[nodiscard]] static constexpr std::optional<index_type> find_first_index()
				{
					return execute_query< typename query_executor::find_first_index >
						([] <auto Index> { return
							trait_predicate_solver<Predicate, probe_index>::template solve<Index>();
						});
				}

				template<template<typename> typename Predicate>
					requires material_agent::template solvable_trait_predicate<Predicate, probe_index>
				[[nodiscard]] static constexpr std::optional<index_type> find_last_index()
				{
					return execute_query< typename query_executor::find_last_index >
						([] <auto Index> { return
							trait_predicate_solver<Predicate, probe_index>::template solve<Index>();
						});
				}

				template<template<typename> typename Predicate>
					requires material_agent::template solvable_trait_predicate<Predicate, probe_index>
				[[nodiscard]] static constexpr auto find_first()
				{
					return execute_query< typename query_executor::template find_first<
						default_content, trait_predicate_solver<Predicate, probe_index>
					> >();
				}

				template<template<typename> typename Predicate>
					requires material_agent::template solvable_trait_predicate<Predicate, probe_index>
				[[nodiscard]] static constexpr auto find_last()
				{
					return execute_query< typename query_executor::template find_last<
						default_content, trait_predicate_solver<Predicate, probe_index>
					> >();
				}

			};

		};

	};

}

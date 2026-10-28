// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/verification/executor/template_verification_executor.h>
#include "agent.h"
#include "error_reporter.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 value_list 的实例
	template<typename List>
	struct value_list_verification_protocol
	{
		using material_agent		= value_list_agent<List>;
		using verification_executor	= template_verification_executor<material_agent>;
		using error_reporter		= value_list_error_reporter;

		template<typename VerificationWorkshop>
		struct extended_core
		{
			using basic_core = typename VerificationWorkshop::basic_core;
			static constexpr bool default_verification_result = VerificationWorkshop::default_verification_result;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::satisfies;

				//================================================================================

				template<template<auto> typename>
				[[nodiscard]] static constexpr bool satisfies() noexcept { return default_verification_result; }

			};

		};

		template<typename VerificationWorkshop>
			requires VerificationWorkshop::constraint::traversable_extent
		struct extended_core<VerificationWorkshop>
		{
			using index_type = typename material_agent::index_type;

			template<template<auto> typename Predicate, index_type ProbeIndex>
			using trait_predicate_solver = typename material_agent::template trait_predicate_solver<Predicate, ProbeIndex>;

			//--------------------------------------------------------------------------------

			using basic_core = typename VerificationWorkshop::basic_core;
			using extent_trait = typename VerificationWorkshop::extent_trait;

			template<typename Executor, typename PredicateSolverType>
			static constexpr auto execute_verification(PredicateSolverType&& predicate_solver)
			{
				return VerificationWorkshop::template execute_verification<Executor>(std::forward<PredicateSolverType>(predicate_solver));
			}

			//--------------------------------------------------------------------------------

			static constexpr auto probe_index = extent_trait::probe_index;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::satisfies;

				//================================================================================

				template<template<auto> typename Verifier>
				[[nodiscard]] static constexpr bool satisfies()
				{
					return execute_verification< typename verification_executor::satisfies >
						(trait_predicate_solver<Verifier, probe_index>{});
				}

			};

		};

	};

}

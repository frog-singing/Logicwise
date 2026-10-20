// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename VerificationWorkshop>
	struct template_verification_component
	{
		static constexpr bool default_verification_result = VerificationWorkshop::default_verification_result;

		//================================================================================

		struct component
		{
			[[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return default_verification_result; }

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{};

	};

	template<typename VerificationWorkshop>
		requires VerificationWorkshop::constraint::traversable_extent
	struct template_verification_component<VerificationWorkshop>
	{
		using material_agent		= typename VerificationWorkshop::material_agent;
		using constraint			= typename VerificationWorkshop::constraint;
		using verification_executor	= typename VerificationWorkshop::verification_executor;
		using error_reporter		= typename VerificationWorkshop::error_reporter;

		template<typename Executor, typename PredicateSolverType>
		static constexpr auto execute_verification(PredicateSolverType&& predicate_solver)
		{
			return VerificationWorkshop::template execute_verification<Executor>
				(std::forward<PredicateSolverType>(predicate_solver));
		}

		//================================================================================

		struct component
		{
			template<typename VerifierType>
				requires constraint::template valid_predicate_type<VerifierType>
			[[nodiscard]] static constexpr bool satisfies(VerifierType&& verifier)
			{
				return execute_verification< typename verification_executor::satisfies >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(verifier);
					});
			}

			//================================================================================

			template<typename VerifierType>
			static constexpr bool satisfies(VerifierType&&)
			{
				error_reporter::template incompatible_verifier<VerifierType>();

				return false;
			}

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{};

	};

}

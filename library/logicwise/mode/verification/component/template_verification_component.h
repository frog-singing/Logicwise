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

		struct elementwise
		{
			[[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return default_verification_result; }

		};

		struct elementwise_with_index {};

		struct rangewise
		{
			[[nodiscard]] static constexpr bool rangewise_satisfies(auto&&) noexcept { return default_verification_result; }

		};

		struct rangewise_with_index {};

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

		//--------------------------------------------------------------------------------

		using computation_operator = typename material_agent::computation_operator;

		//================================================================================

		struct elementwise
		{
			template<typename VerifierType>
				requires constraint::elementwise::template valid_predicate<VerifierType>
			[[nodiscard]] static constexpr bool satisfies(VerifierType&& verifier)
			{
				return execute_verification< typename verification_executor::satisfies >
					(typename computation_operator::elementwise::instance_predicate_solver{ verifier });
			}

			//================================================================================

			template<typename VerifierType>
			static constexpr bool satisfies(VerifierType&&)
			{
				error_reporter::elementwise::template incompatible_verifier<VerifierType>();

				return false;
			}

		};

		struct elementwise_with_index {};

		struct rangewise
		{
			template<typename VerifierType>
				requires constraint::rangewise::template valid_predicate<VerifierType>
			[[nodiscard]] static constexpr bool rangewise_satisfies(VerifierType&& verifier)
			{
				return execute_verification< typename verification_executor::satisfies >
					(typename computation_operator::rangewise::instance_predicate_solver{ verifier });
			}

			//================================================================================

			template<typename VerifierType>
			static constexpr bool rangewise_satisfies(VerifierType&&)
			{
				error_reporter::rangewise::template incompatible_verifier<VerifierType>();

				return false;
			}

		};

		struct rangewise_with_index {};

	};

}

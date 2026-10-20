// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/verification/component/template_verification_constraint_component.h>
#include <logicwise/mode/verification/component/template_verification_component.h>
#include "type.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename VerificationProtocol, typename Quantifier, typename Arrangement>
	struct virtual_template_verification : virtual_template_verification_workshop_tag
	{
		//前向声明 
		struct constraint;
		struct basic_core;
		struct verification;

		//--------------------------------------------------------------------------------

		using material_agent		= typename VerificationProtocol::material_agent;
		using verification_executor	= typename VerificationProtocol::verification_executor;
		using error_reporter		= typename VerificationProtocol::error_reporter;

		static constexpr bool default_verification_result{ typename Quantifier::solver{}.result() };
		using extent_trait = typename material_agent::template extent_trait<Arrangement>;

		template<typename Executor, typename PredicateSolverType>
		static constexpr auto execute_verification(PredicateSolverType&& predicate_solver)
		{
			return Executor::template execute<Quantifier, Arrangement>
				(std::forward<PredicateSolverType>(predicate_solver));
		}

		//--------------------------------------------------------------------------------

		using ConstraintComponent = template_verification_constraint_component<material_agent, Arrangement>;

		struct constraint
			: ConstraintComponent::component
		{};

		//--------------------------------------------------------------------------------

		using VerificationComponent = template_verification_component<virtual_template_verification>;

		struct basic_core
			: VerificationComponent::component
		{};

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] verification
			: resolve_core<VerificationProtocol, virtual_template_verification>
			, virtual_template_verification_product_tag
		{};

	};

}

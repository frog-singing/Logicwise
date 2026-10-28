// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/verification/component/template_verification_constraint.h>
#include <logicwise/mode/verification/component/template_verification_component.h>
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename VerificationProtocol, typename Quantifier, typename Arrangement>
	struct template_verification_workshop
	{
		//前向声明
		struct verification;

		//--------------------------------------------------------------------------------

		using material_agent		= typename VerificationProtocol::material_agent;
		using verification_executor	= typename VerificationProtocol::verification_executor;
		using error_reporter		= typename VerificationProtocol::error_reporter;

		static constexpr bool default_verification_result{ typename Quantifier::solver{}.result() };
		using extent_trait = typename material_agent::template extent_trait<Arrangement>;

		template<typename Executor, typename... Argument>
		static constexpr auto execute_verification(Argument&&... argument)
		{
			return Executor::template execute<Quantifier, Arrangement>(
				std::forward<Argument>(argument)...
			);
		}

		//--------------------------------------------------------------------------------

		using constraint = typename template_verification_constraint<material_agent, Arrangement>::constraint;

		//--------------------------------------------------------------------------------

		using basic_core = resolve_basic_core<
			material_agent,
			template_verification_component<template_verification_workshop>
		>;

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] verification
			: resolve_core<VerificationProtocol, template_verification_workshop>
		{
			using role_tag				= product_role_tag;
			using mode_tag				= verification_mode_tag;
			using material_domain_tag	= template_material_domain_tag;
			using organization_form_tag	= typename material_agent::organization_form_tag;
			using material_feeding_tag	= typename material_agent::material_feeding_tag;
		};

	};

}

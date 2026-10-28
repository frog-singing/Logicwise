// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/traversal/component/template_traversal_constraint.h>
#include <logicwise/mode/traversal/component/template_traversal_component.h>
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename TraversalProtocol, typename Arrangement, typename IndexTraverserType>
	struct template_traversal_workshop
	{
		//前向声明
		struct traversal;

		//--------------------------------------------------------------------------------

		using material_agent		= typename TraversalProtocol::material_agent;
		using traversal_executor	= typename TraversalProtocol::traversal_executor;
		using error_reporter		= typename TraversalProtocol::error_reporter;

		using extent_trait = typename material_agent::template extent_trait<Arrangement, IndexTraverserType>;

		template<typename Executor, typename... Argument>
		static constexpr auto execute_traversal(Argument&&... argument)
		{
			return Executor::template execute<Arrangement, IndexTraverserType>(
				std::forward<Argument>(argument)...
			);
		}

		//--------------------------------------------------------------------------------

		using constraint = typename template_traversal_constraint<material_agent, Arrangement>::constraint;

		//--------------------------------------------------------------------------------

		using basic_core = resolve_basic_core<
			material_agent,
			template_traversal_component<template_traversal_workshop>
		>;

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] traversal
			: resolve_core<TraversalProtocol, template_traversal_workshop>
		{
			using role_tag				= product_role_tag;
			using mode_tag				= traversal_mode_tag;
			using material_domain_tag	= template_material_domain_tag;
			using organization_form_tag	= typename material_agent::organization_form_tag;
			using material_feeding_tag	= typename material_agent::material_feeding_tag;
		};

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/query/component/template_query_constraint.h>
#include <logicwise/mode/query/component/template_query_component.h>
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename QueryProtocol, typename Arrangement>
	struct template_query_workshop
	{
		//前向声明
		struct query;

		//--------------------------------------------------------------------------------

		using material_agent	= typename QueryProtocol::material_agent;
		using query_executor	= typename QueryProtocol::query_executor;
		using default_content	= typename QueryProtocol::default_content;
		using error_reporter	= typename QueryProtocol::error_reporter;

		using extent_trait = typename material_agent::template extent_trait<Arrangement>;

		template<typename Executor, typename... Argument>
		static constexpr auto execute_query(Argument&&... argument)
		{
			return Executor::template execute<Arrangement>(
				std::forward<Argument>(argument)...
			);
		}

		//--------------------------------------------------------------------------------

		using constraint = typename template_query_constraint<material_agent, Arrangement>::constraint;

		//--------------------------------------------------------------------------------

		using basic_core = resolve_basic_core<
			material_agent,
			template_query_component<template_query_workshop>
		>;

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] query
			: resolve_core<QueryProtocol, template_query_workshop>
		{
			using role_tag				= product_role_tag;
			using mode_tag				= query_mode_tag;
			using material_domain_tag	= template_material_domain_tag;
			using organization_form_tag	= typename material_agent::organization_form_tag;
			using material_feeding_tag	= typename material_agent::material_feeding_tag;
		};

	};

}

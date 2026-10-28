// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/view/component/template_view_constraint.h>
#include <logicwise/mode/view/component/template_view_component.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	template<typename ViewProtocol, auto Data>
	struct template_view_workshop
	{
		//前向声明
		struct view;

		//--------------------------------------------------------------------------------

		using material_agent	= typename ViewProtocol::material_agent;
		using error_reporter	= typename ViewProtocol::error_reporter;

		using extent_trait = view_extent_trait<Data>;

		template<typename Adaptor>
		using new_view = typename template_view_workshop< ViewProtocol, Adaptor::template adapt<Data>() >::view;

		//--------------------------------------------------------------------------------

		using constraint = typename template_view_constraint<material_agent, Data>::constraint;

		//--------------------------------------------------------------------------------

		using basic_core = resolve_basic_core<
			material_agent,
			template_view_component<template_view_workshop>
		>;

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] view
			: resolve_core<ViewProtocol, template_view_workshop>
			, ViewProtocol::template materializer<Data>
		{
			using role_tag				= product_role_tag;
			using mode_tag				= view_mode_tag;
			using material_domain_tag	= template_material_domain_tag;
			using organization_form_tag	= typename material_agent::organization_form_tag;
			using material_feeding_tag	= typename material_agent::material_feeding_tag;
		};

	};

}

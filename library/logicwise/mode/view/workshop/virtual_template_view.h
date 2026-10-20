// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/view/component/template_view_constraint_component.h>
#include <logicwise/mode/view/component/template_view_component.h>
#include "type.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	template<typename ViewProtocol, auto Data>
	struct virtual_template_view : virtual_template_view_workshop_tag
	{
		//前向声明
		struct constraint;
		struct basic_core;
		struct view;

		//--------------------------------------------------------------------------------

		using material_agent	= typename ViewProtocol::material_agent;
		using error_reporter	= typename ViewProtocol::error_reporter;

		using extent_trait = view_extent_trait<Data>;

		template<typename Adaptor>
		using new_view = typename virtual_template_view< ViewProtocol, Adaptor::template adapt<Data>() >::view;

		//--------------------------------------------------------------------------------

		using ConstraintComponent = template_view_constraint_component<material_agent, Data>;

		struct constraint
			: ConstraintComponent::component
		{};

		//--------------------------------------------------------------------------------

		using DependentViewComponent = template_view_component<virtual_template_view>;

		struct basic_core
			: DependentViewComponent::component
		{};

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] view
			: resolve_core<ViewProtocol, virtual_template_view>
			, ViewProtocol::template materializer<Data>
			, virtual_template_view_product_tag
		{};

	};

}

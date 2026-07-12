// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/view/component/view_constraint_component.h>
#include <logicwise/mode/view/component/non_dependent_view_component.h>
#include <logicwise/mode/view/component/dependent_view_component.h>
#include "type.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename ViewProtocol, auto Data>
	struct concrete_view
	{
		using material_agent			= typename ViewProtocol::material_agent;
		using dependent_view_adaptor	= typename ViewProtocol::dependent_view_adaptor;
		using error_reporter			= typename ViewProtocol::error_reporter;

		template<typename Adaptor>
		using new_view = concrete_view< ViewProtocol, Adaptor::template adapt<Data>() >::view;

		using ViewConstraintComponent = view_constraint_component<material_agent, Data>;

		struct constraint
			: ViewConstraintComponent::component
			, ViewConstraintComponent::component_with_index
		{};

		using DependentViewComponent = dependent_view_component<concrete_view>;

		struct view
			: non_dependent_view_component<concrete_view>::component
			, DependentViewComponent::component
			, DependentViewComponent::component_with_index
			, ViewProtocol::template materializer<Data>
			, concrete_view_tag
		{};

	};

}

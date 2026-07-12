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
	struct virtual_view
	{
		using material_agent			= typename ViewProtocol::material_agent;
		using dependent_view_adaptor	= typename ViewProtocol::dependent_view_adaptor;
		using error_reporter			= typename ViewProtocol::error_reporter;

		template<typename Adaptor>
		using new_view = virtual_view< ViewProtocol, Adaptor::template adapt<Data>() >::view;

		struct constraint
			: view_constraint_component<material_agent, Data>::component
		{};

		struct view
			: non_dependent_view_component	<virtual_view>::component
			, dependent_view_component		<virtual_view>::component
			, ViewProtocol::template materializer<Data>
			, virtual_view_tag
		{};

	};

}

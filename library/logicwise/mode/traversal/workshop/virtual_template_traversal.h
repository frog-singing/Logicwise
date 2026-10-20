// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/traversal/component/template_traversal_constraint_component.h>
#include <logicwise/mode/traversal/component/template_traversal_component.h>
#include "type.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename TraversalProtocol, typename Arrangement, typename IndexTraverserType>
	struct virtual_template_traversal : virtual_template_traversal_workshop_tag
	{
		//前向声明 
		struct constraint;
		struct basic_core;
		struct traversal;

		//--------------------------------------------------------------------------------

		using material_agent		= typename TraversalProtocol::material_agent;
		using traversal_executor	= typename TraversalProtocol::traversal_executor;
		using error_reporter		= typename TraversalProtocol::error_reporter;

		using extent_trait = typename material_agent::template extent_trait<Arrangement, IndexTraverserType>;

		template<typename Executor>
		static constexpr auto execute_traversal()
		{
			return Executor::template execute<Arrangement, IndexTraverserType>();
		}

		template<typename Executor, typename OperationExecutorType>
		static constexpr auto execute_traversal(OperationExecutorType&& operation_executor)
		{
			return Executor::template execute<Arrangement, IndexTraverserType>
				(std::forward<OperationExecutorType>(operation_executor));
		}

		//--------------------------------------------------------------------------------

		using ConstraintComponent = template_traversal_constraint_component<material_agent, Arrangement>;

		struct constraint
			: ConstraintComponent::component
		{};

		//--------------------------------------------------------------------------------

		using TraversalComponent = template_traversal_component<virtual_template_traversal>;

		struct basic_core
			: TraversalComponent::component
		{};

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] traversal
			: resolve_core<TraversalProtocol, virtual_template_traversal>
			, virtual_template_traversal_product_tag
		{};

	};

}

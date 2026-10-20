// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/resolve_core.h>
#include <logicwise/mode/query/component/template_query_constraint_component.h>
#include <logicwise/mode/query/component/template_query_component.h>
#include "type.h"
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename QueryProtocol, typename Arrangement>
	struct virtual_template_query : virtual_template_query_workshop_tag
	{
		//前向声明 
		struct constraint;
		struct basic_core;
		struct query;

		//--------------------------------------------------------------------------------

		using material_agent	= typename QueryProtocol::material_agent;
		using query_executor	= typename QueryProtocol::query_executor;
		using default_content	= typename QueryProtocol::default_content;
		using error_reporter	= typename QueryProtocol::error_reporter;

		using extent_trait = typename material_agent::template extent_trait<Arrangement>;

		template<typename Executor>
		static constexpr auto execute_query()
		{
			return Executor::template execute<Arrangement>();
		}

		template<typename Executor, typename PredicateSolverType>
		static constexpr auto execute_query(PredicateSolverType&& predicate_solver)
		{
			return Executor::template execute<Arrangement>
				(std::forward<PredicateSolverType>(predicate_solver));
		}

		//--------------------------------------------------------------------------------

		using ConstraintComponent = template_query_constraint_component<material_agent, Arrangement>;

		struct constraint
			: ConstraintComponent::component
		{};

		//--------------------------------------------------------------------------------

		using QueryComponent = template_query_component<virtual_template_query>;

		struct basic_core
			: QueryComponent::component
		{};

		//--------------------------------------------------------------------------------

		struct [[nodiscard]] query
			: resolve_core<QueryProtocol, virtual_template_query>
			, virtual_template_query_product_tag
		{};

	};

}

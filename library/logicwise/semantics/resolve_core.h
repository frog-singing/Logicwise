// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//语义 semantics================================================================================

	template<typename MaterialAgent, typename ProductComponent> struct basic_core_resolver;

	template<typename MaterialAgent, typename ProductComponent>
		requires ConcreteOrganizationForm<MaterialAgent>&& PackableMaterialFeeding<MaterialAgent>
	struct basic_core_resolver<MaterialAgent, ProductComponent>
	{
		struct basic_core
			: ProductComponent::elementwise
			, ProductComponent::elementwise_with_index
			, ProductComponent::rangewise
			, ProductComponent::rangewise_with_index
		{};
	};

	template<typename MaterialAgent, typename ProductComponent>
		requires ConcreteOrganizationForm<MaterialAgent> && NonPackableMaterialFeeding<MaterialAgent>
	struct basic_core_resolver<MaterialAgent, ProductComponent>
	{
		struct basic_core
			: ProductComponent::elementwise
			, ProductComponent::elementwise_with_index
		{};
	};

	template<typename MaterialAgent, typename ProductComponent>
		requires VirtualOrganizationForm<MaterialAgent> && PackableMaterialFeeding<MaterialAgent>
	struct basic_core_resolver<MaterialAgent, ProductComponent>
	{
		struct basic_core
			: ProductComponent::elementwise
			, ProductComponent::rangewise
		{};
	};

	template<typename MaterialAgent, typename ProductComponent>
		requires VirtualOrganizationForm<MaterialAgent> && NonPackableMaterialFeeding<MaterialAgent>
	struct basic_core_resolver<MaterialAgent, ProductComponent>
	{
		struct basic_core
			: ProductComponent::elementwise
		{};
	};

	template<typename MaterialAgent, typename ProductComponent>
	using resolve_basic_core = typename basic_core_resolver<MaterialAgent, ProductComponent>::basic_core;

	//--------------------------------------------------------------------------------

	template<typename Protocol, typename Workshop>
	struct core_resolver
	{
		using core = typename Workshop::basic_core;
	};

	template<typename Protocol, typename Workshop>
		requires requires { typename Protocol::template extended_core<Workshop>::core; }
	struct core_resolver<Protocol, Workshop>
	{
		using core = typename Protocol::template extended_core<Workshop>::core;
	};

	template<typename Protocol, typename Workshop>
	using resolve_core = typename core_resolver<Protocol, Workshop>::core;

}

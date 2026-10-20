// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	//遍历车间标签
	struct traversal_workshop_tag
	{
		using role_tag = workshop_role_tag;
		using mode_tag = traversal_mode_tag;
	};

	struct template_traversal_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_traversal_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_traversal_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_traversal_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//遍历车间约束
	template<typename Object> concept TraversalWorkshop = Workshop<Object> && TraversalMode<Object>;

	template<typename Object> concept TemplateTraversalWorkshop =
		Workshop<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object>;

	template<typename Object> concept InstanceTraversalWorkshop =
		Workshop<Object> && TraversalMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateTraversalWorkshop =
		Workshop<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateTraversalWorkshop =
		Workshop<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

	//--------------------------------------------------------------------------------

	//遍历产品标签
	struct traversal_product_tag
	{
		using role_tag = product_role_tag;
		using mode_tag = traversal_mode_tag;
	};

	struct template_traversal_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_traversal_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_traversal_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_traversal_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= traversal_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//遍历产品约束
	template<typename Object> concept Traversal = Product<Object> && TraversalMode<Object>;
	template<typename Object> concept TemplateTraversal = Product<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object>;
	template<typename Object> concept InstanceTraversal = Product<Object> && TraversalMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateTraversal =
		Product<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateTraversal =
		Product<Object> && TraversalMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

}

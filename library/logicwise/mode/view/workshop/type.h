// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	//视图车间标签
	struct view_workshop_tag
	{
		using role_tag = workshop_role_tag;
		using mode_tag = view_mode_tag;
	};

	struct concrete_template_view_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= view_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_view_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= view_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//视图车间约束
	template<typename Object> concept ViewWorkshop = Workshop<Object> && ViewMode<Object>;

	template<typename Object> concept ConcreteTemplateViewWorkshop =
		Workshop<Object> && ViewMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateViewWorkshop =
		Workshop<Object> && ViewMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

	//--------------------------------------------------------------------------------

	//视图产品标签
	struct view_product_tag
	{
		using role_tag = product_role_tag;
		using mode_tag = view_mode_tag;
	};

	struct concrete_template_view_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= view_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_view_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= view_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//视图产品约束
	template<typename Object> concept View = Product<Object> && ViewMode<Object>;

	template<typename Object> concept ConcreteTemplateView =
		Product<Object> && ViewMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateView =
		Product<Object> && ViewMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

}

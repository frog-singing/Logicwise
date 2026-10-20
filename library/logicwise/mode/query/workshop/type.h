// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	//查询车间标签
	struct query_workshop_tag
	{
		using role_tag = workshop_role_tag;
		using mode_tag = query_mode_tag;
	};

	struct template_query_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_query_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_query_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_query_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//查询车间约束
	template<typename Object> concept QueryWorkshop = Workshop<Object> && QueryMode<Object>;

	template<typename Object> concept TemplateQueryWorkshop =
		Workshop<Object> && QueryMode<Object> && TemplateMaterialDomain<Object>;

	template<typename Object> concept InstanceQueryWorkshop =
		Workshop<Object> && QueryMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateQueryWorkshop =
		Workshop<Object> && QueryMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateQueryWorkshop =
		Workshop<Object> && QueryMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

	//--------------------------------------------------------------------------------

	//查询产品标签
	struct query_product_tag
	{
		using role_tag = product_role_tag;
		using mode_tag = query_mode_tag;
	};

	struct template_query_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_query_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_query_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_query_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= query_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//查询产品约束
	template<typename Object> concept Query = Product<Object> && QueryMode<Object>;
	template<typename Object> concept TemplateQuery = Product<Object> && QueryMode<Object> && TemplateMaterialDomain<Object>;
	template<typename Object> concept InstanceQuery = Product<Object> && QueryMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateQuery =
		Product<Object> && QueryMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateQuery =
		Product<Object> && QueryMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/semantics/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	//验证车间标签
	struct verification_workshop_tag
	{
		using role_tag = workshop_role_tag;
		using mode_tag = verification_mode_tag;
	};

	struct template_verification_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_verification_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_verification_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_verification_workshop_tag
	{
		using role_tag				= workshop_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//验证车间约束
	template<typename Object> concept VerificationWorkshop = Workshop<Object> && VerificationMode<Object>;

	template<typename Object> concept TemplateVerificationWorkshop =
		Workshop<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object>;

	template<typename Object> concept InstanceVerificationWorkshop =
		Workshop<Object> && VerificationMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateVerificationWorkshop =
		Workshop<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateVerificationWorkshop =
		Workshop<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

	//--------------------------------------------------------------------------------

	//验证产品标签
	struct verification_product_tag
	{
		using role_tag = product_role_tag;
		using mode_tag = verification_mode_tag;
	};

	struct template_verification_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
	};

	struct instance_verification_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= instance_material_domain_tag;
	};

	struct concrete_template_verification_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
	};

	struct virtual_template_verification_product_tag
	{
		using role_tag				= product_role_tag;
		using mode_tag				= verification_mode_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= virtual_organization_form_tag;
	};

	//验证产品约束
	template<typename Object> concept Verification = Product<Object> && VerificationMode<Object>;
	template<typename Object> concept TemplateVerification = Product<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object>;
	template<typename Object> concept InstanceVerification = Product<Object> && VerificationMode<Object> && InstanceMaterialDomain<Object>;

	template<typename Object> concept ConcreteTemplateVerification =
		Product<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object> && ConcreteOrganizationForm<Object>;

	template<typename Object> concept VirtualTemplateVerification =
		Product<Object> && VerificationMode<Object> && TemplateMaterialDomain<Object> && VirtualOrganizationForm<Object>;

}

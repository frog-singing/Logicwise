// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::convertible_to，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//语义 semantics================================================================================

	//角色标签
	struct role_tag {};
	struct mode_option_role_tag : role_tag {};		//模式选项角色
	//struct arrangement_role_tag : role_tag {};	//排布角色
	//struct wrapper_role_tag : role_tag {};		//包装器角色
	struct material_agent_role_tag : role_tag {};	//物料代理角色
	struct protocol_role_tag : role_tag {};			//协议角色
	struct workshop_role_tag : role_tag {};			//车间角色
	struct product_role_tag : role_tag {};			//产品角色

	//模式标签
	struct mode_tag {};
	struct verification_mode_tag : mode_tag {};		//验证模式
	struct traversal_mode_tag : mode_tag {};		//遍历模式
	struct query_mode_tag : mode_tag {};			//查询模式
	struct view_mode_tag : mode_tag {};				//视图模式

	//物料论域标签
	struct material_domain_tag {};
	struct template_material_domain_tag : material_domain_tag {};	//模板物料论域
	struct instance_material_domain_tag : material_domain_tag {};	//实例物料论域

	//组织形式标签
	struct organization_form_tag {};
	struct concrete_organization_form_tag : organization_form_tag {};	//实体组织形式
	struct virtual_organization_form_tag : organization_form_tag {};	//虚拟组织形式

	//物料给料标签
	struct material_feeding_tag {};
	struct packable_material_feeding_tag : material_feeding_tag {};		//可打包物料给料
	struct non_packable_material_feeding_tag : material_feeding_tag {};	//不可打包物料给料

	//--------------------------------------------------------------------------------

	//角色约束
	template <typename Object, typename Tag>
	concept RoleOf =
		//std::derived_from<Tag, role_tag> &&
		requires { requires std::derived_from<typename Object::role_tag, Tag>; };

	template <typename Object> concept ModeOption	= RoleOf<Object, mode_option_role_tag>;
	template <typename Object> concept Workshop		= RoleOf<Object, workshop_role_tag>;
	template <typename Object> concept Product		= RoleOf<Object, product_role_tag>;

	//模式约束
	template <typename Object, typename Tag>
	concept ModeOf =
		//std::derived_from<Tag, mode_tag> &&
		requires { requires std::derived_from<typename Object::mode_tag, Tag>; };

	template <typename Object> concept VerificationMode	= ModeOf<Object, verification_mode_tag>;
	template <typename Object> concept TraversalMode	= ModeOf<Object, traversal_mode_tag>;
	template <typename Object> concept QueryMode		= ModeOf<Object, query_mode_tag>;
	template <typename Object> concept ViewMode			= ModeOf<Object, view_mode_tag>;

	//物料论域约束
	template <typename Object, typename Tag>
	concept MaterialDomainOf =
		//std::derived_from<Tag, material_domain_tag> &&
		requires { requires std::derived_from<typename Object::material_domain_tag, Tag>; };

	template <typename Object> concept TemplateMaterialDomain	= MaterialDomainOf<Object, template_material_domain_tag>;
	template <typename Object> concept InstanceMaterialDomain	= MaterialDomainOf<Object, instance_material_domain_tag>;

	//组织形式约束
	template <typename Object, typename Tag>
	concept OrganizationFormOf =
		//std::derived_from<Tag, organization_form_tag> &&
		requires { requires std::derived_from<typename Object::organization_form_tag, Tag>; };

	template <typename Object> concept ConcreteOrganizationForm	= OrganizationFormOf<Object, concrete_organization_form_tag>;
	template <typename Object> concept VirtualOrganizationForm	= OrganizationFormOf<Object, virtual_organization_form_tag>;

	//物料给料约束
	template <typename Object, typename Tag>
	concept MaterialFeedingOf =
		//std::derived_from<Tag, material_feeding_tag> &&
		requires { requires std::derived_from<typename Object::material_feeding_tag, Tag>; };

	template <typename Object> concept PackableMaterialFeeding		= MaterialFeedingOf<Object, packable_material_feeding_tag>;
	template <typename Object> concept NonPackableMaterialFeeding	= MaterialFeedingOf<Object, non_packable_material_feeding_tag>;

}

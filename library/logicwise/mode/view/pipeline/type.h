// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::derived_from，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	//视图标签
	struct view_tag {};
	struct concrete_view_tag	: view_tag {};
	struct virtual_view_tag		: view_tag {};

	//视图约束
	template<typename TargetView>
	concept View = std::derived_from<TargetView, view_tag>;

	template<typename View>
	concept ConcreteView	= std::derived_from<View, concrete_view_tag>;

	template<typename View>
	concept VirtualView		= std::derived_from<View, virtual_view_tag>;

}

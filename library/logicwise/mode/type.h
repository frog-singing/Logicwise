// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/index/index_traverser_selector.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式 mode================================================================================

	//行为模式标签
	struct traversal_mode_tag {};
	struct query_mode_tag {};
	struct view_mode_tag {};

	//行为模式约束
	template <typename Mode>
	concept TraversalMode = std::derived_from<Mode, traversal_mode_tag>;

	template <typename Mode>
	concept QueryMode = std::derived_from<Mode, query_mode_tag>;

	template <typename Mode>
	concept ViewMode = std::derived_from<Mode, view_mode_tag>;

	//遍历
	struct for_each				: traversal_mode_tag, forward_index_traverser_selector {};
	struct in_reverse_for_each	: traversal_mode_tag, reverse_index_traverser_selector {};
	
	//查询
	struct query : query_mode_tag {};

	//视图化
	struct view				: view_mode_tag, forward_index_traverser_selector {};
	struct in_reverse_view	: view_mode_tag, reverse_index_traverser_selector {};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/semantics/tag.h>
#include <logicwise/index/index_traverser_selector.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//模式 mode================================================================================

	//模式选项标签
	struct traversal_mode_option_tag	{ using role_tag = mode_option_role_tag; using mode_tag = traversal_mode_tag;	};
	struct query_mode_option_tag		{ using role_tag = mode_option_role_tag; using mode_tag = query_mode_tag;		};
	struct view_mode_option_tag			{ using role_tag = mode_option_role_tag; using mode_tag = view_mode_tag;		};

	//模式选项约束
	template <typename Object> concept TraversalModeOption	= ModeOption<Object> && TraversalMode<Object>;
	template <typename Object> concept QueryModeOption		= ModeOption<Object> && QueryMode<Object>;
	template <typename Object> concept ViewModeOption		= ModeOption<Object> && ViewMode<Object>;

	//遍历 traverse
	struct for_each				: traversal_mode_option_tag, forward_index_traverser_selector {};
	struct in_reverse_for_each	: traversal_mode_option_tag, reverse_index_traverser_selector {};
	
	//查询 query
	struct query : query_mode_option_tag {};

	//视图化 view
	struct view				: view_mode_option_tag, forward_index_traverser_selector {};
	struct in_reverse_view	: view_mode_option_tag, reverse_index_traverser_selector {};

}

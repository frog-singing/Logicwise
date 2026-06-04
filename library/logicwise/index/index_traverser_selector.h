// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	struct forward_index_traverser_selector
	{
		template<typename Arrangement>
		using index_traverser = typename Arrangement::forward_index_traverser;
	};

	struct reverse_index_traverser_selector
	{
		template<typename Arrangement>
		using index_traverser = typename Arrangement::reverse_index_traverser;
	};

	struct fast_index_traverser_selector
	{
		template<typename Arrangement>
		using index_traverser = typename Arrangement::fast_index_traverser;
	};

	struct light_index_traverser_selector
	{
		template<typename Arrangement>
		using index_traverser = typename Arrangement::light_index_traverser;
	};

}

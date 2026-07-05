// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/vector_like_container.h>
#include <logicwise/semantics/padding/template_element_padder.h>
#include <logicwise/semantics/padding/padding_instance_trait.h>
#include "view_loop.h"
#include "view_bake.h"
#include <ranges> //用于 std::ranges，C++20标准
#include <functional> //用于 std::invoke
#include <utility> //用于 std::forward


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class bipartite_viewing;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	//(PaddingElementI, PaddingElementJ) 必须是合法组合

	template<typename Mode, ArrangementWithPadding Arrangement>
	class bipartite_viewing<Mode, Arrangement> : area_ahead<Mode, Arrangement>
	{


		// : area_ahead<Mode, Arrangement>


	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/mode/type.h>
#include <logicwise/mode/rangewise_verification.h>
#include <logicwise/mode/rangewise_traversal.h>
#include <logicwise/mode/rangewise_querying.h>
#include <logicwise/mode/rangewise_viewing.h>
#include <logicwise/arrangement/type.h>
#include <concepts> //用于 std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//范围维度 rangewise================================================================================

	template<typename ModeOption, typename Arrangement>
	struct rangewise_impl
	{
		static_assert(dependent_false_v<ModeOption>, "[logicwise] Invalid ModeOption.");
	};

	template<Quantifier ModeOption, typename Arrangement>
	struct rangewise_impl<ModeOption, Arrangement>
	{
		using type = rangewise_verification<ModeOption, Arrangement>;
	};

	template<TraversalModeOption ModeOption, typename Arrangement>
	struct rangewise_impl<ModeOption, Arrangement>
	{
		using type = rangewise_traversal<ModeOption, Arrangement>;
	};

	template<QueryModeOption ModeOption, typename Arrangement>
	struct rangewise_impl<ModeOption, Arrangement>
	{
		using type = rangewise_querying<ModeOption, Arrangement>;
	};

	template<ViewModeOption ModeOption, typename Arrangement>
	struct rangewise_impl<ModeOption, Arrangement>
	{
		using type = rangewise_viewing<ModeOption, Arrangement>;
	};


	template<typename ModeOption, Arrangement TargetArrangement>
	using rangewise = typename rangewise_impl<ModeOption, TargetArrangement>::type;

}

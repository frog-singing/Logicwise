// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/type.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/mode/rangewise_validation.h>
#include <logicwise/mode/rangewise_query.h>
#include <logicwise/mode/rangewise_view.h>
#include <logicwise/external_detail/exosuit.h>
#include <concepts> //用于 std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//范围维度 rangewise================================================================================

	template<typename Mode, typename Arrangement>
	struct rangewise_impl
	{
		static_assert(dependent_false_v<Mode>, "[logicwise] Invalid Mode.");
	};

	template<Quantifier Mode, typename Arrangement>
	struct rangewise_impl<Mode, Arrangement>
	{
		using type = rangewise_validation<Mode, Arrangement>;
	};

	template<std::same_as<query> Mode, typename Arrangement>
	struct rangewise_impl<Mode, Arrangement>
	{
		using type = rangewise_query<Arrangement>;
	};

	template<std::same_as<view> Mode, typename Arrangement>
	struct rangewise_impl<Mode, Arrangement>
	{
		using type = rangewise_view<Arrangement>;
	};


	template<typename Mode, Arrangement TargetArrangement>
	using rangewise = typename rangewise_impl<Mode, TargetArrangement>::type;

}

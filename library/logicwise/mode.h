// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "mode/type.h"


//逻辑维度::量词
namespace logicwise::quantifier
{
	//行为模式::验证 mode::validation================================================================================

	using detail::quantifier_tag;
	using detail::Quantifier;

	using detail::always_true;	// ⊤
	using detail::always_false;	// ⊥

	using detail::all_of;		// ∀
	using detail::any_of;		// ∃
	using detail::none_of;		// ¬∃
	using detail::not_every;	// ¬∀

	using detail::exactly;		// =
	using detail::at_least;		// ≥
	using detail::at_most;		// ≤

	using detail::more_than;	// >
	using detail::less_than;	// <

}


//逻辑维度::行为模式
namespace logicwise::mode
{
	//行为模式 mode================================================================================

	//[未实现] [unimplemented]

	//遍历
	using detail::for_each;
	using detail::in_reverse_for_each;

	//查询
	using detail::query;

	//视图化
	using detail::view;
	using detail::in_reverse_view;

}

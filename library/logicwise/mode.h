// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "mode/type.h"


//模式选项 mode option================================================================================

//逻辑维度::量词
namespace logicwise::quantifier
{
	//验证 verify
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

//逻辑维度::模式
namespace logicwise::mode
{
	//遍历 traverse
	using detail::for_each;
	using detail::in_reverse_for_each;

	//查询 query
	using detail::query;

	//视图化 view
	using detail::view;
	using detail::in_reverse_view;

}

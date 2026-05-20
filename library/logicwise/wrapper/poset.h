// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "poset/type_poset.h"
#include "poset/value_poset.h"


//包装器::偏序集 wrapper::partially ordered set================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_poset;
	using detail::custom_type_poset;
	using detail::as_type_poset;

	using detail::value_poset;
	using detail::custom_value_poset;
	using detail::as_value_poset;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::Poset;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::Poset;

}

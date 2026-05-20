// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "set/type_set.h"
#include "set/value_set.h"


//包装器::集合 wrapper::set================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_set;
	using detail::custom_type_set;
	using detail::as_type_set;

	using detail::value_set;
	using detail::custom_value_set;
	using detail::as_value_set;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::Set;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::Set;

}

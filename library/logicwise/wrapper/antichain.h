// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "antichain/type_antichain.h"
#include "antichain/value_antichain.h"


//包装器::反链 wrapper::antichain================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_antichain;
	using detail::custom_type_antichain;
	using detail::as_type_antichain;

	using detail::value_antichain;
	using detail::custom_value_antichain;
	using detail::as_value_antichain;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::Antichain;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::Antichain;

}

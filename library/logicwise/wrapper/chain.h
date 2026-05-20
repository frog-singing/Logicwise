// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "chain/type_chain.h"
#include "chain/value_chain.h"


//包装器::链 wrapper::chain================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_chain;
	using detail::custom_type_chain;
	using detail::as_type_chain;

	using detail::value_chain;
	using detail::custom_value_chain;
	using detail::as_value_chain;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::Chain;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::Chain;

}

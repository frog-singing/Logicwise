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

	using detail::value_set;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::set; //包装器 wrapper
	using detail::typewise::Set; //概念 concept

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::set; //包装器 wrapper
	using detail::valuewise::Set; //概念 concept

}

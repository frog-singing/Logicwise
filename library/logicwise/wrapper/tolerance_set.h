// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "tolerance_set/type_tolerance_set.h"
#include "tolerance_set/value_tolerance_set.h"


//包装器::相容集 wrapper::tolerance set================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_tolerance_set;

	using detail::value_tolerance_set;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::tolerance_set; //包装器 wrapper
	using detail::typewise::ToleranceSet; //概念 concept

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::tolerance_set; //包装器 wrapper
	using detail::valuewise::ToleranceSet; //概念 concept

}

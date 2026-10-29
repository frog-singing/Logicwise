// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "tolerance_class/type_tolerance_class.h"
#include "tolerance_class/value_tolerance_class.h"


//包装器::相容类 wrapper::tolerance class================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_tolerance_class;

	using detail::value_tolerance_class;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::tolerance_class; //包装器 wrapper
	using detail::typewise::ToleranceClass; //概念 concept

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::tolerance_class; //包装器 wrapper
	using detail::valuewise::ToleranceClass; //概念 concept

}

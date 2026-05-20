// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "equivalence_class/type_equivalence_class.h"
#include "equivalence_class/value_equivalence_class.h"


//包装器::等价类 wrapper::equivalence class================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_equivalence_class;
	using detail::custom_type_equivalence_class;
	using detail::as_type_equivalence_class;

	using detail::value_equivalence_class;
	using detail::custom_value_equivalence_class;
	using detail::as_value_equivalence_class;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::EquivalenceClass;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::EquivalenceClass;

}

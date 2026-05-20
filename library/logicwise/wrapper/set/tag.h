// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/wrapper/tolerance_set/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::集合 wrapper::set================================================================================

	//标签
	struct set_tag {};
	struct type_set_tag : set_tag, type_tolerance_set_tag {};
	struct value_set_tag : set_tag, value_tolerance_set_tag {};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/wrapper/set/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::偏序集 wrapper::partially ordered set================================================================================

	//标签
	struct poset_tag {};
	struct type_poset_tag : poset_tag, type_set_tag {};
	struct value_poset_tag : poset_tag, value_set_tag {};

}

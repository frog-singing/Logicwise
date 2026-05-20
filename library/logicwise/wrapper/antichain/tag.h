// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/wrapper/poset/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::反链 wrapper::antichain================================================================================

	//标签
	struct antichain_tag {};
	struct type_antichain_tag : antichain_tag, type_poset_tag {};
	struct value_antichain_tag : antichain_tag, value_poset_tag {};

}

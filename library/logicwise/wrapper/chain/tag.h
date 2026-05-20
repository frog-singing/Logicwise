// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/wrapper/poset/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::链 wrapper::chain================================================================================

	//标签
	struct chain_tag {};
	struct type_chain_tag : chain_tag, type_poset_tag {};
	struct value_chain_tag : chain_tag, value_poset_tag {};

}

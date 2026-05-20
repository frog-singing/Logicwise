// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/wrapper/tolerance_class/tag.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::等价类 wrapper::equivalence class================================================================================

	//标签
	struct equivalence_class_tag {};
	struct type_equivalence_class_tag : equivalence_class_tag, type_tolerance_class_tag {};
	struct value_equivalence_class_tag : equivalence_class_tag, value_tolerance_class_tag {};

}

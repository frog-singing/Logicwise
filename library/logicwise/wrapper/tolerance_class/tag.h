// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::相容类 wrapper::tolerance class================================================================================
	
	//标签
	struct tolerance_class_tag {};
	struct type_tolerance_class_tag : tolerance_class_tag {};
	struct value_tolerance_class_tag : tolerance_class_tag {};

}

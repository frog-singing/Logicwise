// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//包装器::相容集 wrapper::tolerance set================================================================================
	
	//标签
	struct tolerance_set_tag {};
	struct type_tolerance_set_tag : tolerance_set_tag {};
	struct value_tolerance_set_tag : tolerance_set_tag {};

}

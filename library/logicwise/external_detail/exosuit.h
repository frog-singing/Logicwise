// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <manipond/exosuit/error/dependent_false.h>
#include <manipond/exosuit/work_in_progress/area_ahead.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//外装件 exosuit================================================================================

	//模板依赖静态断言组件，用于报错
	using manipond::exosuit::dependent_false_v;

	using manipond::exosuit::area_ahead;
	using manipond::exosuit::explore_later;

}

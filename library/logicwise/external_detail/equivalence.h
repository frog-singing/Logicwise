// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <manipond/meta/constraint/equivalence.h>


//等价 equivalence================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	using manipond::meta::typewise::same_as;

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	using manipond::meta::valuewise::same_as;
	using manipond::meta::valuewise::equal_to;

}

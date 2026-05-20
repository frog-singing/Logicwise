// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>


//包装器::范围 wrapper::range================================================================================

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	template<typename TargetRange>
	concept Range = List<TargetRange>;

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	template<typename TargetRange>
	concept Range = List<TargetRange>;

}


//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::Range;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::Range;

}

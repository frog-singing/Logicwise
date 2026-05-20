// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <manipond/meta/wrapper/list.h>
#include <manipond/meta/trait/list.h>


//列表 list================================================================================

//逻辑维度::细节
namespace logicwise::detail
{
	using manipond::meta::type_list;
	using manipond::meta::as_type_list;
	using manipond::meta::head_t;
	using manipond::meta::t_tail_list;

	using manipond::meta::value_list;
	using manipond::meta::as_value_list;
	using manipond::meta::head_v;
	using manipond::meta::v_tail_list;

	using manipond::meta::type_extractor;
	using manipond::meta::decl_type;

}

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	using manipond::meta::typewise::List;

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	using manipond::meta::valuewise::List;

}

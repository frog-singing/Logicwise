// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>


//包装器::列表 wrapper::list================================================================================

//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::type_list;
	using detail::as_type_list;

	using detail::value_list;
	using detail::as_value_list;

}

//逻辑维度::元映射
namespace logicwise::trait
{
	using detail::head_t;
	using detail::t_tail_list;

	using detail::head_v;
	using detail::v_tail_list;

	using detail::type_extractor;
	using detail::decl_type;

}

//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::List;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::List;

}

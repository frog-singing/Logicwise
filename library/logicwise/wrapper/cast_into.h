// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>


//包装器 wrapper================================================================================

//逻辑维度::细节
namespace logicwise::detail
{
	template<typename Range, typename Wrapper>
	struct cast_into_impl
	{
		static_assert(dependent_false_v<Range>,
			"[logicwise] Error: Incompatible Range!\n"
			"Expected: pure type list or pure value list.");
	};

	template<typewise::List Range, typename Wrapper>
	struct cast_into_impl<Range, Wrapper>
	{
		using range = typename as_type_list<Range>::template apply<Wrapper::template wrapping>;
	};

	template<valuewise::List Range, typename Wrapper>
	struct cast_into_impl<Range, Wrapper>
	{
		using range = typename as_value_list<Range>::template apply<Wrapper::template wrapping>;
	};

	template<typename Range, typename Wrapper>
	using cast_into = typename cast_into_impl<Range, Wrapper>::range;

}


//逻辑维度::包装器
namespace logicwise::wrapper
{
	using detail::cast_into;

}

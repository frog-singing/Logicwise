// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//语义 semantics================================================================================

	template<typename Protocol, typename Workshop>
	struct core_resolver
	{
		using core = typename Workshop::basic_core;
	};

	template<typename Protocol, typename Workshop>
		requires requires { typename Protocol::template extended_core<Workshop>::core; }
	struct core_resolver<Protocol, Workshop>
	{
		using core = typename Protocol::template extended_core<Workshop>::core;
	};

	template<typename Protocol, typename Workshop>
	using resolve_core = typename core_resolver<Protocol, Workshop>::core;

}

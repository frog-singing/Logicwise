// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <variant> //用于 std::monostate


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	struct value_list_default_content
	{
		static constexpr auto null_object = std::monostate{};

	};

}

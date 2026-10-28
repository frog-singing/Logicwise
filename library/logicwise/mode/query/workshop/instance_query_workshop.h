// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once

#include "type.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename QueryProtocol, typename Arrangement>
	struct instance_query_workshop : instance_query_workshop_tag
	{



		struct [[nodiscard]] query
			: instance_query_product_tag
		{


		};

	};

}

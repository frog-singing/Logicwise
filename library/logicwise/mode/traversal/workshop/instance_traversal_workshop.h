// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once

#include "type.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename TraversalProtocol, typename IndexTraverserSelector, typename Arrangement>
	struct instance_traversal_workshop : instance_traversal_workshop_tag
	{



		struct [[nodiscard]] traversal
			: instance_traversal_product_tag
		{


		};

	};

}

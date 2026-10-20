// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once

#include "type.h"


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

	template<typename VerificationProtocol, typename Quantifier, typename Arrangement>
	struct instance_verification : instance_verification_workshop_tag
	{



		struct [[nodiscard]] verification
			: instance_verification_product_tag
		{


		};

	};

}

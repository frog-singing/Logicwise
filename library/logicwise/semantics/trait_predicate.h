// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>


//逻辑维度::细节
namespace logicwise::detail
{
    //语义 semantics================================================================================

    template<typename TraitCertificate>
	inline constexpr auto trait_predicate_solver = [] <typename TraitInstance> {

		if constexpr		( requires { bool{	TraitCertificate::value     };  }	)
							{ return			TraitInstance::value;			    }

		else if constexpr	( requires { bool{	TraitCertificate{}	        };  }   )
		                    { return            TraitInstance{};                    }

		else
		{
			static_assert(dependent_false_v<TraitCertificate>,
				"[logicwise] Error: Unsupported trait predicate!\n"
				"Certificate must provide '::value' or be convertible to bool via construction.");

			return false;
		}
	};

}

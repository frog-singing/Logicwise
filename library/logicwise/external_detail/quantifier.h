// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <manipond/meta/algorithm/predicate/quantifier.h>


//逻辑维度::细节
namespace logicwise::detail
{
	//量词 quantifier================================================================================

	using manipond::meta::predicate::quantifier_tag;
	using manipond::meta::predicate::Quantifier;

	using manipond::meta::predicate::always_true;	// ⊤
	using manipond::meta::predicate::always_false;	// ⊥

	using manipond::meta::predicate::all_of;		// ∀
	using manipond::meta::predicate::any_of;		// ∃
	using manipond::meta::predicate::none_of;		// ¬∃
	using manipond::meta::predicate::not_every;		// ¬∀

	using manipond::meta::predicate::exactly;		// =
	using manipond::meta::predicate::at_least;		// ≥
	using manipond::meta::predicate::at_most;		// ≤

	using manipond::meta::predicate::more_than;		// >
	using manipond::meta::predicate::less_than;		// <

}

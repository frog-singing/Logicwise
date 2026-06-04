// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	struct CoffeePot		{ static consteval bool I_can_brew_coffee() { return true; } };
	struct CoffeeMachine	{ static consteval bool I_can_brew_coffee() { return true; } };
	struct BrokenCoffeePot	{ static consteval bool I_can_brew_coffee() { return false; } };

	struct Teapot
	{
		// cannot be called at compile time
		static bool I_can_brew_coffee()
		{
			std::cout << "I'm a teapot" << std::endl;
			return false;
		}
	};

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier; // check this out in logicwise/mode.h
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper;
	
	using namespace logicwise::test;

	//--------------------------------------------------------------------------------

	// Normally, predicates are validated in reverse order.
	// In case of elementwise arrangement, predicates are always validated in reverse order.

	static_assert(
		rangewise<always_true, element>
		::in<type_list<Teapot, Teapot, Teapot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: always_true"
	);

	static_assert(
		not
		rangewise<always_false, element>
		::in<type_list<Teapot, Teapot, Teapot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: always_false"
	);

	static_assert(
		not
		rangewise<all_of, element>
		::in<type_list<Teapot, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: all_of"
	);

	static_assert(
		rangewise<any_of, element>
		::in<type_list<Teapot, CoffeePot, BrokenCoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: any_of"
	);

	static_assert(
		not
		rangewise<none_of, element>
		::in<type_list<Teapot, CoffeePot, BrokenCoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: none_of"
	);

	static_assert(
		rangewise<not_every, element>
		::in<type_list<Teapot, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: not_every"
	);

	static_assert(
		not
		rangewise<exactly<1>, element>
		::in<type_list<Teapot, CoffeeMachine, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: exactly<N>"
	);

	static_assert(
		rangewise<at_least<2>, element>
		::in<type_list<Teapot, CoffeeMachine, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: at_least<N>"
	);

	static_assert(
		not
		rangewise<at_most<1>, element>
		::in<type_list<Teapot, CoffeeMachine, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: at_most<N>"
	);

	static_assert(
		rangewise<more_than<1>, element>
		::in<type_list<Teapot, CoffeeMachine, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: more_than<N>"
	);

	static_assert(
		not
		rangewise<less_than<2>, element>
		::in<type_list<Teapot, CoffeeMachine, BrokenCoffeePot, CoffeePot>>()
		.satisfies([] <typename SomePot> { return SomePot::I_can_brew_coffee(); }),
		"short-circuit evaluation: less_than<N>"
	);

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

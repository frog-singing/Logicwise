// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <concepts> // std::same_as
#include <cstddef> // std::size_t
#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier; // check this out in logicwise/mode.h
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper;

	// quantifier semantics --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, int, int, int>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier semantics: all_of"
	);
	// try to replace all_of with any_of

	static_assert(
		rangewise<none_of, element>
		::in<type_list<bool, double*, float&&, const int&>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier semantics: none_of"
	);
	// try to replace none_of with not_every

	static_assert(
		rangewise<exactly<2>, element>
		::in<type_list<int&&, int, int*&, int, const int>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier semantics: exactly<N>"
	);
	// try to replace exactly<2> with at_least<2>, at_most<2>, more_than<1>, less_than<3>


	static_assert(
		rangewise<always_true, element>
		::in<type_list<bool, double*, float&&, const int&>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier semantics: always_true"
	);

	static_assert(
		not
		rangewise<always_false, element>
		::in<type_list<int, int, int, int>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier semantics: always_false"
	);

	// quantifier vacuity --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, element>
		::in<type_list< >>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier vacuity: all_of"
	);

	static_assert(
		rangewise<none_of, element>
		::in<type_list< >>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier vacuity: none_of"
	);

	static_assert(
		not
		rangewise<any_of, element>
		::in<type_list< >>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier vacuity: any_of"
	);

	static_assert(
		not
		rangewise<not_every, element>
		::in<type_list< >>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier vacuity: not_every"
	);

	static_assert(
		rangewise<exactly<0>, element>
		::in<type_list< >>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"quantifier vacuity: exactly<0>"
	);

	// quantifier identity --------------------------------------------------------------------------------

	static_assert(
		rangewise<quantifier::all_of, arrangement::combination_pair>
		::in<wrapper::type_list<always_true, at_least<0>>>()
		.satisfies([] <typename Q1, typename Q2> { return std::same_as<Q1, Q2>; }),
		"quantifier identity: always_true"
	);

	static_assert(
		rangewise<quantifier::all_of, arrangement::combination_pair>
		::in<wrapper::type_list<always_false, less_than<0>>>()
		.satisfies([] <typename Q1, typename Q2> { return std::same_as<Q1, Q2>; }),
		"quantifier identity: always_false"
	);

	static_assert(
		rangewise<quantifier::all_of, arrangement::combination_pair>
		::in<wrapper::type_list<any_of, more_than<0>, at_least<1>>>()
		.satisfies([] <typename Q1, typename Q2> { return std::same_as<Q1, Q2>; }),
		"quantifier identity: any_of"
	);

	static_assert(
		rangewise<quantifier::all_of, arrangement::combination_pair>
		::in<wrapper::type_list<none_of, exactly<0>, at_most<0>, less_than<1>>>()
		.satisfies([] <typename Q1, typename Q2> { return std::same_as<Q1, Q2>; }),
		"quantifier identity: none_of"
	);


	static constexpr std::size_t M{ 3 };
	// try to replace M with other non-negative integers

	// detail::IndexTraitScalar::index_sequence<0, 1, 2, ..., M - 1>
	using natural_numbers = detail::index_sequencer<
		arrangement::element,
		arrangement::element::forward_index_traverser,
		arrangement::element::extent_type{ M }
	>::index_sequence;

	static_assert(
		rangewise<quantifier::all_of, arrangement::element>
		::in<natural_numbers>() // { 0, 1, 2, ..., M - 1 }
		.satisfies([] <auto N> { return std::same_as<more_than<N>, at_least<N + 1>>; }),
		"quantifier identity: more_than<N>, at_least<N + 1>"
	);

	static_assert(
		rangewise<quantifier::all_of, arrangement::element>
		::in<natural_numbers>() // { 0, 1, 2, ..., M - 1 }
		.satisfies([] <auto N> { return std::same_as<at_most<N>, less_than<N + 1>>; }),
		"quantifier identity: at_most<N>, less_than<N + 1>"
	);

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <array> // std::array

#include <cstddef> // std::size_t
#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper;

	//--------------------------------------------------------------------------------

	static constexpr std::size_t N{ 3 };

	using quantifier_list = type_list<
		always_true, always_false,
		all_of, any_of, none_of, not_every,
		exactly<N>, at_least<N>, at_most<N>,
		more_than<N>, less_than<N>
	>;

	using unreachable_predicate_list = value_list<
		[] <typename SomeTeapot> { return SomeTeapot::Im_a_teapot; },
		[] <auto SomeValue> { return SomeValue.some_function(); },
		[] (auto&& some_instance) { return some_function(some_instance); },
		[] <typename T1, typename T2> { return T1::template some_trait<T2>::value; },
		[] <auto V1, auto V2> { return V1.template operator() < V2 > () && V2.template operator() < V1 > (); },
		[] (auto&& i1, auto&& i2) { return i1.will_never_use(i2); },
		[] <typename Type, auto Value> { return Type{}(Value) && Value(Type{}); },
		[] <auto CanYouBrewCoffee, typename MaybeATeapot>
			{ return CanYouBrewCoffee.template operator() < MaybeATeapot > (); },
		[] <auto WhoAreYou>(auto&& some_instance) { return WhoAreYou(some_instance); },
		[] <typename ActuallyIDontCare>(auto&& some_instance) { return ActuallyIDontCare{ some_instance }; },
		[] { return true; },
		[] { struct A { int x{}; }; return A{ 42 }; },
		[] {}, 42, nullptr
	>;

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between<quantifier_list, unreachable_predicate_list>()
		.satisfies([] <typename Quantifier, auto UnreachablePredicate> {

			using arrangement_list = type_list<element>;
			using non_traversable_range_list = type_list<
				type_list< >, value_list< >
			>;

			return rangewise<all_of, cartesian_pair>
				::between<arrangement_list, non_traversable_range_list>()
				.satisfies([] <typename Arrangement, typename NonTraversableRange> { return

					rangewise<Quantifier, Arrangement>
					::template in<NonTraversableRange>()
					.satisfies(UnreachablePredicate)
					==
					typename Quantifier::solver{}.result();
				});
		}),
		"elementwise non-traversable extent: element"
	);

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between<quantifier_list, unreachable_predicate_list>()
		.satisfies([] <typename Quantifier, auto UnreachablePredicate> {

			using arrangement_list = type_list<
				permutation_pair, combination_pair, linear_adjacent_pair
			>;
			using non_traversable_range_list = type_list<
				type_list< >, value_list< >, type_list<void>, value_list<nullptr>
			>;

			return rangewise<all_of, cartesian_pair>
				::between<arrangement_list, non_traversable_range_list>()
				.satisfies([] <typename Arrangement, typename NonTraversableRange> { return

					rangewise<Quantifier, Arrangement>
					::template in<NonTraversableRange>()
					.satisfies(UnreachablePredicate)
					==
					typename Quantifier::solver{}.result();
				});
		}),
		"pairwise non-traversable extent: permutation_pair, combination_pair, linear_adjacent_pair"
	);

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between<quantifier_list, unreachable_predicate_list>()
		.satisfies([] <typename Quantifier, auto UnreachablePredicate> {

			using arrangement_list = type_list<circular_adjacent_pair>;
			using non_traversable_range_list = type_list<
				type_list< >, value_list< >
			>;

			return rangewise<all_of, cartesian_pair>
				::between<arrangement_list, non_traversable_range_list>()
				.satisfies([] <typename Arrangement, typename NonTraversableRange> { return

					rangewise<Quantifier, Arrangement>
					::template in<NonTraversableRange>()
					.satisfies(UnreachablePredicate)
					==
					typename Quantifier::solver{}.result();
				});
		}),
		"pairwise non-traversable extent: circular_adjacent_pair"
	);

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between<quantifier_list, unreachable_predicate_list>()
		.satisfies([] <typename Quantifier, auto UnreachablePredicate> {
			
			using arrangement_list = type_list<
				cartesian_pair, zip_pair_truncation
			>;
			using non_traversable_range_list = type_list<
				type_list< >, value_list< >
			>;
			using traversable_range_list = type_list<
				type_list<void>, value_list<nullptr>
			>;
			std::array<int, 0> non_traversable_range{};
			std::array traversable_range{ nullptr };

			constexpr bool default_validation_result{ typename Quantifier::solver{}.result() };

			return rangewise<all_of, element>
				::in<arrangement_list>()
				.satisfies([&] <typename Arrangement> { return

					rangewise<all_of, cartesian_pair>
					::between<non_traversable_range_list, traversable_range_list>()
					.satisfies([&] <typename NonTraversableRange, typename TraversableRange> { return

						rangewise<Quantifier, Arrangement>
						::template between<NonTraversableRange, TraversableRange>()
						.satisfies(UnreachablePredicate)
						==
						default_validation_result &&

						rangewise<Quantifier, Arrangement>
						::template between<TraversableRange, NonTraversableRange>()
						.satisfies(UnreachablePredicate)
						==
						default_validation_result &&

						rangewise<Quantifier, Arrangement>
						::template between<NonTraversableRange>(traversable_range)
						.satisfies(UnreachablePredicate)
						==
						default_validation_result &&

						rangewise<Quantifier, Arrangement>
						::template between<TraversableRange>(non_traversable_range)
						.satisfies(UnreachablePredicate)
						==
						default_validation_result;
					});
				});
		}),
		"bipartite non-traversable extent: cartesian_pair, zip_pair_truncation"
	);

	// No predicate form constraint degeneracy for bipartite arrangement zip_pair_padding.
	// Since padding elements are provided explicitly,
	// the predicate is always constrained in terms of form,
	// regardless of the traversability of the extent.
	// Thus, predicates for zip_pair_padding should always be well-formed.

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

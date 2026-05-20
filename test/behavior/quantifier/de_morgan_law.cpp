// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <concepts> // std::same_as, std::derived_from, std::convertible_to
#include <type_traits> // std::true_type
#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper;

	//--------------------------------------------------------------------------------

	inline constexpr auto logical_conjunction = [] <valuewise::List PredicateList> { return
		[] <typename VariableList> { return
			rangewise<all_of, element> // all_of: ∀
			::template in<PredicateList>()
			.satisfies([] <auto Predicate> { return Predicate.template operator() < VariableList > (); });
		};
	};

	inline constexpr auto logical_disjunction = [] <valuewise::List PredicateList> { return
		[] <typename VariableList> { return
			rangewise<any_of, element> // any_of: ∃
			::template in<PredicateList>()
			.satisfies([] <auto Predicate> { return Predicate.template operator() < VariableList > (); });
		};
	};

	inline constexpr auto logical_conjunction_of_negation = [] <valuewise::List PredicateList> { return
		[] <typename VariableList> { return
			rangewise<none_of, element> // none_of: ∀¬
			::template in<PredicateList>()
			.satisfies([] <auto Predicate> { return Predicate.template operator() < VariableList > (); });
		};
	};

	inline constexpr auto logical_disjunction_of_negation = [] <valuewise::List PredicateList> { return
		[] <typename VariableList> { return
			rangewise<not_every, element> // not_every: ∃¬
			::template in<PredicateList>()
			.satisfies([] <auto Predicate> { return Predicate.template operator() < VariableList > (); });
		};
	};

	//--------------------------------------------------------------------------------

	struct A { int x{}; };

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper; // including type_value_pair

	using namespace logicwise::test;

	//--------------------------------------------------------------------------------

	using type_value_pair_list = type_list<
		type_value_pair<int, 42>,
		type_value_pair<A, A{42}>,
		type_value_pair<A, nullptr>,
		std::true_type // { std::integral_constant<bool, true>, true }
	>;
	// try to add more type-value pairs

	//--------------------------------------------------------------------------------

	static constexpr auto consistency = [] <typename T, auto V> {
		return std::same_as<T, decltype(V)>;
	};

	static constexpr auto inheritance = [] <typename T, auto V> {
		return std::derived_from<decltype(V), T>;
	};

	static constexpr auto convertibility = [] <typename T, auto V> {
		return std::convertible_to<decltype(V), T>;
	};

	static constexpr auto constructibility = [] <typename T, auto V> {
		return requires { T{ V }; };
	};

	// try to add more type-value relations

	static constexpr auto to_property = [] <auto Relation> { return
		[] <typename TypeValuePair> {
			return Relation.template operator()
				< typename TypeValuePair::type, TypeValuePair::value > ();
		};
	};

	using property_list = value_list<
		consistency,
		inheritance,
		convertibility,
		constructibility
	>::transform<to_property>;

	//--------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, element>
		::in<type_value_pair_list>()
		.satisfies([] <typename TypeValuePair> { return
			not logical_conjunction
			.template operator() < property_list > ()
			.template operator() < TypeValuePair > ()
			==
			logical_disjunction_of_negation
			.template operator() < property_list > ()
			.template operator() < TypeValuePair > ();
		}),
		"De Morgan's first law: not (P1 and P2 and ... and Pn) ≡ (not P1) or (not P2) or ... or (not Pn)"
	);

	static_assert(
		rangewise<all_of, element>
		::in<type_value_pair_list>()
		.satisfies([] <typename TypeValuePair> { return
			not logical_disjunction
			.template operator() < property_list > ()
			.template operator() < TypeValuePair > ()
			==
			logical_conjunction_of_negation
			.template operator() < property_list > ()
			.template operator() < TypeValuePair > ();
		}),
		"De Morgan's second law: not (P1 or P2 or ... or Pn) ≡ (not P1) and (not P2) and ... and (not Pn)"
	);

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

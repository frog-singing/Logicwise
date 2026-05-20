// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <tuple> // std::tuple
#include <variant> // std::variant

#include <concepts> // std::same_as
#include <cstddef> // std::size_t
#include <utility> // std::move
#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	struct rock; struct paper; struct scissors;

	struct rock		{ using beats = scissors; };
	struct paper	{ using beats = rock; };
	struct scissors	{ using beats = paper; };

	enum struct outcome { win, lose, tie };

	template<typename Move1, typename Move2> struct match;

	template<typename Move1, typename Move2>
		requires std::same_as<typename Move1::beats, Move2>
	struct match<Move1, Move2>
	{
		static constexpr auto result{ outcome::win };
	};

	template<typename Move1, typename Move2>
		requires std::same_as<typename Move2::beats, Move1>
	struct match<Move1, Move2>
	{
		static constexpr auto result{ outcome::lose };
	};

	template<typename Move>
	struct match<Move, Move>
	{
		static constexpr auto result{ outcome::tie };
	};

	//--------------------------------------------------------------------------------

	template<typename Move>
	concept ValidMove =
		rangewise<quantifier::any_of, arrangement::element>
		::in<wrapper::type_list<rock, paper, scissors>>()
		.satisfies([] <typename PresetMove> { return std::same_as<Move, PresetMove>; });

	template<typename... Move> struct move_sequence {};

	//--------------------------------------------------------------------------------

	struct more_than_half_of : quantifier::quantifier_tag
	{
		struct solver
		{
			std::size_t count{ 0 };
			std::size_t total{ 0 };

			solver() = default;

			constexpr bool solved() const noexcept { return false; } // never early solved
			constexpr bool result() const noexcept { return count > total / 2; }

			constexpr void step(bool condition) noexcept
			{
				count += static_cast<std::size_t>(condition);
				++total;
			}
		};
	};

	//--------------------------------------------------------------------------------

	template<auto Type_Predicate, typename... Type>
	struct is_type_predicate_satisfied_with_value
	{
		static constexpr bool value{ Type_Predicate.template operator() < Type... > () };
	};

	template<auto Type_Predicate, typename... Type>
	struct is_type_predicate_satisfied_with_operator_bool
	{
		constexpr operator bool() const noexcept
		{
			return Type_Predicate.template operator() < Type... > ();
		};
	};

	template<typename T, auto V>
	struct is_type_value_pair_consistent_with_value
	{
		static constexpr bool value{ std::same_as<T, decltype(V)> };
	};

	template<typename T, auto V>
	struct is_type_value_pair_consistent_with_operator_bool
	{
		constexpr operator bool() const noexcept { return std::same_as<T, decltype(V)>; };
	};

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h
	using namespace logicwise::wrapper;

	using namespace logicwise::test;

	// type wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<exactly<3 * 3>, cartesian_pair>
		::between<type_list<rock, paper, scissors>, std::tuple<scissors, rock, paper>>()
		.satisfies([] <typename M1, typename M2> { return true; }),
		"type wrapper: rock-paper-scissors matrix extent"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<outcome::win, outcome::lose, outcome::tie>>()
		.satisfies([] <auto Outcome> { return
			rangewise<exactly<3>, cartesian_pair>
			::between<type_list<rock, paper, scissors>, std::variant<scissors, rock, paper>>()
			.satisfies([] <typename M1, typename M2> { return match<M1, M2>::result == Outcome; });
		}),
		"type wrapper: rock-paper-scissors match matrix"
	);

	using move_sequence_A = move_sequence<rock, paper, scissors>;
	using move_sequence_B = move_sequence<rock, paper, rock, paper, scissors>;

	static_assert(
		rangewise<more_than_half_of, zip_pair_truncation>
		::between<move_sequence_A, move_sequence_B>()
		.satisfies([] <typename M1, typename M2> { return match<M1, M2>::result == outcome::tie; }),
		"type wrapper: a boring game"
	);

	// only rock will be used for padding since move_sequence_A is shorter
	static_assert(
		rangewise<more_than_half_of, zip_pair_padding>
		::with_padding<rock, paper>()
		.between<move_sequence_A, move_sequence_B>()
		.satisfies([] <typename M1, typename M2> { return match<M1, M2>::result != outcome::tie; }),
		"type wrapper: an exciting game"
	);

	// value wrapper and type wrapper --------------------------------------------------------------------------------

	static constexpr auto size_requirement = [] <typename Sequence> {
		return as_type_list<Sequence>::size >= 3;
	};

	static constexpr auto move_requirement = [] <typename Sequence> {
		return rangewise<all_of, element>
			::template in<Sequence>()
			.satisfies([] <typename Move> { return ValidMove<Move>; });
	};

	static constexpr auto adjacency_requirement = [] <typename Sequence> {
		return rangewise<none_of, linear_adjacent_pair>
			::template in<Sequence>()
			.satisfies([] <typename M1, typename M2> { return std::same_as<M1, M2>; });
	};

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between<
			value_list<size_requirement, move_requirement, adjacency_requirement>,
			type_list<move_sequence_A, move_sequence_B>
		>()
		.satisfies([] <auto Requirement, typename Sequence> {
			return Requirement.template operator() < Sequence > ();
		}),
		"value wrapper and type wrapper: requirements complied via lambda"
	);


	using move_sequence_audit_policy = rangewise<all_of, cartesian_pair>;

	static constexpr auto move_sequence_audit_scenario =
		move_sequence_audit_policy::between<
			value_list<size_requirement, move_requirement, adjacency_requirement>,
			type_list<move_sequence_A, move_sequence_B>
		>();

	static_assert(
		move_sequence_audit_scenario
		.satisfies<is_type_predicate_satisfied_with_value>(),
		"value wrapper and type wrapper: requirements complied via trait with value"
	);
	
	static_assert(
		move_sequence_audit_scenario
		.satisfies<is_type_predicate_satisfied_with_operator_bool>(),
		"value wrapper and type wrapper: requirements complied via trait with operator bool"
	);

	// value wrapper --------------------------------------------------------------------------------

	using my_value_pool = value_list<50, 40u, 30.0>;
	using your_value_pool = value_list<'0', 35, false>; // '0' is 48 in ASCII

	static_assert(
		rangewise<more_than_half_of, cartesian_pair>
		::between<my_value_pool, your_value_pool>()
		.satisfies([] <auto V1, auto V2> { return V1 > V2; }),
		"value wrapper: I have a high expectation of winning"
	);

	using my_value_sequence =
		value_list< get<0>(my_value_pool{}), get<1>(my_value_pool{}) >;

	using your_value_sequence =
		value_list< get<2>(your_value_pool{}), get<0>(your_value_pool{}), get<1>(your_value_pool{})> ;

	static_assert(
		rangewise<exactly<1>, zip_pair_truncation>
		::between<my_value_sequence, your_value_sequence>()
		.satisfies([] <auto V1, auto V2> { return V1 > V2; }),
		"value wrapper: ties in the first two rounds"
	);

	// only get<2>(my_value_pool{}) will be used for padding since my_value_sequence is shorter
	static_assert(
		rangewise<more_than_half_of, zip_pair_padding>
		::with_padding< get<2>(my_value_pool{}), get<1>(your_value_pool{}) >()
		.between<my_value_sequence, your_value_sequence>()
		.satisfies([] <auto V1, auto V2> { return V1 < V2; }),
		"value wrapper: you win"
	);

	// type wrapper and value wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, zip_pair_truncation>
		::between<type_list<bool, char, int>, your_value_sequence>()
		.satisfies([] <typename T, auto V> { return std::same_as<T, decltype(V)>; }),
		"type wrapper and value wrapper: type-value consistency via lambda"
	);


	using value_sequence_audit_policy = rangewise<all_of, zip_pair_truncation>;

	static constexpr auto value_sequence_audit_scenario =
		value_sequence_audit_policy::between<type_list<bool, char, int>, your_value_sequence>();

	static_assert(
		value_sequence_audit_scenario
		.satisfies<is_type_value_pair_consistent_with_value>(),
		"type wrapper and value wrapper: type-value consistency via trait with value"
	);
	
	static_assert(
		value_sequence_audit_scenario
		.satisfies<is_type_value_pair_consistent_with_operator_bool>(),
		"type wrapper and value wrapper: type-value consistency via trait with operator bool"
	);

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

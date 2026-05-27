// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <variant> // std::variant, std::visit
#include <array> // std::array
#include <span> // std::span

#include <concepts> // std::same_as
#include <type_traits> // std::is_same
#include <utility> // std::move
#include <iostream> // std::cout, std::boolalpha, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	struct spring; struct summer; struct autumn; struct winter;

	template<typename Season> struct season_trait;

	template<> struct season_trait<spring> { using next_season = summer; };
	template<> struct season_trait<summer> { using next_season = autumn; };
	template<> struct season_trait<autumn> { using next_season = winter; };
	template<> struct season_trait<winter> { using next_season = spring; };

	template<typename ThisSeason>
	struct season
	{
		using next_season = typename season_trait<ThisSeason>::next_season;
		int year;
	};

	struct spring : season<spring>
	{ constexpr int year_of_next_season() const noexcept { return year; } };

	struct summer : season<summer>
	{ constexpr int year_of_next_season() const noexcept { return year; } };

	struct autumn : season<autumn>
	{ constexpr int year_of_next_season() const noexcept { return year; } };

	struct winter : season<winter>
	{ constexpr int year_of_next_season() const noexcept { return year + 1; } };

	//--------------------------------------------------------------------------------

	template<typename T1, typename T2>
	struct is_same_with_operator_bool
	{
		constexpr operator bool() const noexcept { return false; }
	};

	template<typename T>
	struct is_same_with_operator_bool<T, T>
	{
		constexpr operator bool() const noexcept { return true; }
	};

	template<auto S1, auto S2>
	struct is_same_year_with_value
	{
		static constexpr bool value{ S1.year == S2.year };
	};

	template<auto S1, auto S2>
	struct is_same_year_with_opeartor_bool
	{
		constexpr operator bool() const noexcept { return S1.year == S2.year; }
	};

	//--------------------------------------------------------------------------------

	enum struct plan_choice { travel, create, rest, study, work, celebrate };

	template<auto SeasonInstance> struct seasonal_plan { plan_choice plan; };

	//--------------------------------------------------------------------------------

	enum struct difficulty_level { easy, medium, hard };

	template<auto SeasonInstance>
	struct seasonal_challenge : seasonal_plan<SeasonInstance>
	{
		difficulty_level difficulty;
		constexpr seasonal_challenge(seasonal_plan<SeasonInstance> p, difficulty_level d)
			: seasonal_plan<SeasonInstance>{ p }, difficulty{ d } {}
	};

	template<auto SeasonInstance>
	seasonal_challenge(seasonal_plan<SeasonInstance>, difficulty_level)
		-> seasonal_challenge<SeasonInstance>;

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h
	using namespace logicwise::wrapper;
	using namespace logicwise::container; // for to_variant_array

	using namespace logicwise::test;

	// type wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<none_of, combination_pair>
		::in<type_list<spring, summer, autumn, winter>>()
		.satisfies([] <typename S1, typename S2> { return std::same_as<S1, S2>; }),
		"type wrapper: distinct seasons via lambda"
	);

	static_assert(
		rangewise<none_of, combination_pair>
		::in<type_list<spring, summer, autumn, winter>>()
		.satisfies<std::is_same>(),
		"type wrapper: distinct seasons via trait with value"
	);

	static_assert(
		rangewise<none_of, combination_pair>
		::in<type_list<spring, summer, autumn, winter>>()
		.satisfies<is_same_with_operator_bool>(),
		"type wrapper: distinct seasons via trait with operator bool"
	);


	static_assert(
		rangewise<all_of, linear_adjacent_pair>
		::in<type_list<spring, summer, autumn>>()
		.satisfies([] <typename S1, typename S2> { return std::same_as<typename S1::next_season, S2>; }),
		"type wrapper: season flow"
	);
	// try to add more following seasons

	static_assert(
		rangewise<all_of, circular_adjacent_pair>
		::in<type_list<autumn, winter, spring, summer>>()
		.satisfies([] <typename S1, typename S2> { return std::same_as<typename S1::next_season, S2>; }),
		"type wrapper: season loop"
	);
	// try to use type_list<autumn, winter, spring, summer, autumn, winter, spring, summer> instead

	static_assert(
		rangewise<exactly<1>, combination_pair>
		::in<type_list<winter, autumn, summer, spring>>()
		.satisfies([] <typename S1, typename S2> { return std::same_as<typename S1::next_season, S2>; }),
		"type wrapper: one-way matching"
	);

	static_assert(
		rangewise<exactly<4>, permutation_pair>
		::in<type_list<winter, autumn, summer, spring>>()
		.satisfies([] <typename S1, typename S2> { return std::same_as<typename S1::next_season, S2>; }),
		"type wrapper: two-way matching"
	);

	// value wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, linear_adjacent_pair>
		::in<value_list<spring{ 2026 }, summer{ 2026 }, autumn{ 2026 }, winter{ 2026 }>> ()
		.satisfies([] <auto S1, auto S2> { return S1.year == S2.year; }),
		"value wrapper: seasons in the same year via lambda"
	);

	static_assert(
		rangewise<all_of, linear_adjacent_pair>
		::in<value_list<spring{ 2026 }, summer{ 2026 }, autumn{ 2026 }, winter{ 2026 }>> ()
		.satisfies<is_same_year_with_value>(),
		"value wrapper: seasons in the same year via trait with value"
	);

	static_assert(
		rangewise<all_of, linear_adjacent_pair>
		::in<value_list<spring{ 2026 }, summer{ 2026 }, autumn{ 2026 }, winter{ 2026 }>> ()
		.satisfies<is_same_year_with_opeartor_bool>(),
		"value wrapper: seasons in the same year via trait with operator bool"
	);


	static constexpr auto abs = [] (int x) { return x >= 0 ? x : -x; };

	static_assert(
		rangewise<all_of, combination_pair>
		::in<value_list<spring{ 2022 }, spring{ 2024 }, spring{ 2026 }, spring{ 2028 }>> ()
		.satisfies([] <auto S1, auto S2> { return abs(S1.year - S2.year) < 10; }),
		"value wrapper: springs within a decade"
	);


	static constexpr auto one_way_adjacency = [] <auto S1, auto S2> {
		return std::same_as<typename decltype(S1)::next_season, decltype(S2)> &&
			S1.year_of_next_season() == S2.year;
	};

	static_assert(
		rangewise<all_of, linear_adjacent_pair>
		::in<value_list<autumn{ 2026 }, winter{ 2026 }, spring{ 2027 }, summer{ 2027 }>> ()
		.satisfies(one_way_adjacency),
		"value wrapper: season instance flow"
	);

	static_assert(
		rangewise<none_of, permutation_pair>
		::in<value_list<summer{ 2027 }, winter{ 2026 }, summer{ 2026 }, winter{ 2025 }>> ()
		.satisfies(one_way_adjacency),
		"value wrapper: scattered season instances"
	);

	// compile-time vector-like container --------------------------------------------------------------------------------

	static constexpr seasonal_plan<spring{ 2026 }> my_plan_pool_for_spring_2026[4]
	{
		{ plan_choice::travel }, { plan_choice::rest }, { plan_choice::study }, { plan_choice::work }
	};

	using value_list_of_seasonal_plan_2026 = value_list<
		my_plan_pool_for_spring_2026[0],
		seasonal_plan<summer{ 2026 }>{ plan_choice::study },
		seasonal_plan<autumn{ 2026 }>{ plan_choice::create },
		seasonal_plan<winter{ 2026 }>{ plan_choice::celebrate }
	>;

	static constexpr auto seasonal_plan_2026 = value_list_of_seasonal_plan_2026{};

	static_assert(
		rangewise<none_of, combination_pair>
		::in(std::move(my_plan_pool_for_spring_2026))
		.satisfies([] (auto&& p1, auto&& p2) { return p1.plan == p2.plan; }),
		"compile-time container: distinct plan choices"
	);

	static_assert(
		rangewise<none_of, linear_adjacent_pair>
		::in(to_variant_array(seasonal_plan_2026))
		.satisfies([] (auto&& v1, auto&& v2) {
			return std::visit([] (auto&& p1, auto&& p2) { return p1.plan == p2.plan; }, v1, v2);
		}),
		"compile-time container: discontinuous seasonal plans"
	);

	// runtime vector-like container --------------------------------------------------------------------------------

	std::cout << std::boolalpha;

	std::array<std::variant<
		seasonal_challenge<spring{ 2026 }>,
		seasonal_challenge<summer{ 2026 }>,
		seasonal_challenge<autumn{ 2026 }>,
		seasonal_challenge<winter{ 2026 }>
	>, 4> seasonal_challenge_2026
	{
		seasonal_challenge{ get<0>(seasonal_plan_2026), difficulty_level::easy },	// travel in spring
		seasonal_challenge{ get<1>(seasonal_plan_2026), difficulty_level::easy },	// study in summer
		seasonal_challenge{ get<2>(seasonal_plan_2026), difficulty_level::medium },	// create in autumn
		seasonal_challenge{ get<3>(seasonal_plan_2026), difficulty_level::easy }	// celebrate in winter
	};

	// studying is hard instead of easy
	std::visit([] (auto&& challenge) {
		challenge.difficulty = difficulty_level::hard;
	}, seasonal_challenge_2026[1]);

	bool most_challenges_not_hard =
		rangewise<quantifier::at_least<3>, arrangement::element>
		::in(std::span{ seasonal_challenge_2026 })
		.satisfies([] (auto&& variant) {
			return std::visit([] (auto&& challenge) {
				return challenge.difficulty != difficulty_level::hard;
			}, variant);
		});

	std::cout << "most challenges not hard: " << most_challenges_not_hard << std::endl;

	const auto& const_ref_of_seasonal_challenge_2026 = seasonal_challenge_2026;

	bool no_continuous_easy_challenges =
		rangewise<none_of, linear_adjacent_pair>
		::in(const_ref_of_seasonal_challenge_2026)
		.satisfies([] (auto&& v1, auto&& v2) {
			return std::visit([] (auto&& c1, auto&& c2) {
				return c1.difficulty == difficulty_level::easy && c2.difficulty == difficulty_level::easy;
			}, v1, v2);
		});

	std::cout << "no continuous easy challenges: " << no_continuous_easy_challenges << std::endl;

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

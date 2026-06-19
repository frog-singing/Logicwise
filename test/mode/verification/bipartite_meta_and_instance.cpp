// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <tuple> // std::tuple, std::make_tuple
#include <variant> // std::variant, std::visit
#include <array> // std::array

#include <concepts> // std::same_as
#include <iostream> // std::cout, std::boolalpha, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	using namespace logicwise::wrapper; // for type_list, type_set, and type_antichain

	//--------------------------------------------------------------------------------

	enum struct color_type { red, orange, yellow, green, cyan, blue, purple };
	enum struct size_type { big, medium, small };

	template<color_type Color>
	struct fruit
	{
		static constexpr color_type color{ Color };
		size_type size{ size_type::medium };
	};

	struct apple		: fruit<color_type::red>		{};
	struct strawberry	: fruit<color_type::red>		{};
	struct orange		: fruit<color_type::orange>	{};
	struct banana		: fruit<color_type::yellow>	{};
	struct mango		: fruit<color_type::yellow>	{};
	struct watermelon	: fruit<color_type::green>	{};
	struct blueberry	: fruit<color_type::blue>	{};
	struct grape		: fruit<color_type::purple>	{};

	using all_fruit_types = type_list<apple, strawberry, orange, banana, mango, watermelon, blueberry, grape>;

	//--------------------------------------------------------------------------------

	struct Tom
	{
		using preferred_fruit_list = type_set<strawberry, banana, mango, watermelon, blueberry>;

		static constexpr std::array disliked_color_array{ color_type::orange, color_type::cyan, color_type::purple };
	};

	struct supermarket
	{
		using preferred_fruit_list = type_antichain<apple, orange, banana, mango, watermelon>;
	};

	struct fruit_stand
	{
		using preferred_fruit_list = std::tuple<strawberry, banana, mango, watermelon, blueberry, grape>;
	};

	struct fruit_company
	{
		using preferred_fruit_list = all_fruit_types;
	};

	using entity_list = type_list<Tom, supermarket, fruit_stand, fruit_company>;

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h
	using namespace logicwise::wrapper;
	using namespace logicwise::container; // for to_variant_array

	using namespace logicwise::test;

	// type wrapper and compile-time array-like container --------------------------------------------------------------------------------

	static_assert(
		rangewise<none_of, cartesian_pair>
		::between<Tom::preferred_fruit_list>(Tom::disliked_color_array)
		.satisfies([] <typename FruitType>(auto&& color) { return FruitType::color == color; }),
		"type wrapper and compile-time container: Tom's preferred fruits and disliked colors"
	);

	static constexpr auto fruit_tuple = std::make_tuple(
		strawberry{ size_type::big },
		banana{ size_type::medium },
		mango{ size_type::big },
		watermelon{ size_type::big },
		blueberry{ size_type::medium }
	);

	static constexpr auto& fruit_tuple_ref = fruit_tuple;

	static_assert(
		rangewise<all_of, zip_pair_truncation>
		::between<Tom::preferred_fruit_list>(to_variant_array(fruit_tuple_ref))
		.satisfies([] <typename FruitType>(auto&& variant) {
			return std::visit([] (auto fruit) {
				return fruit.size != size_type::small && std::same_as<FruitType, decltype(fruit)>;
			}, variant);
		}),
		"type wrapper and compile-time container: fruits for Tom"
	);

	// value wrapper and compile-time array-like container --------------------------------------------------------------------------------

	using fruit_list = value_list<
		apple{ size_type::medium },
		watermelon{ size_type::big },
		blueberry{ size_type::medium }
	>;

	static_assert(
		rangewise<none_of, cartesian_pair>
		::between<fruit_list>(Tom::disliked_color_array)
		.satisfies([] <auto Fruit>(auto&& color) { return Fruit.color == color || Fruit.size == size_type::small; }),
		"value wrapper and compile-time container: fruits Tom won't dislike"
	);

	static_assert(
		rangewise<all_of, zip_pair_truncation>
		::between<fruit_list>(std::array{ color_type::red, color_type::green, color_type::blue })
		.satisfies([] <auto Fruit>(auto&& color) { return Fruit.color == color; }),
		"value wrapper and compile-time container: an RGB fruit sequence"
	);

	// type wrapper and runtime array-like container --------------------------------------------------------------------------------

	std::cout << std::boolalpha;

	using variant_type = all_fruit_types::template apply<std::variant>;

	std::array<variant_type, 6> fruit_array{
		apple{ size_type::small },
		banana{ size_type::medium },
		mango{ size_type::big },
		mango{ size_type::medium },
		watermelon{ size_type::big },
		grape{ size_type::small }
	};

	fruit_array[0] = banana{ size_type::big };
	fruit_array[5] = watermelon{ size_type::big };

	bool everyone_prefers_these_fruits =
		rangewise<all_of, cartesian_pair>
		::between<entity_list>(fruit_array)
		.satisfies([] <typename Entity>(auto&& variant) {
			return std::visit([] (auto fruit) {
				return typewise::BelongsTo<decltype(fruit), typename Entity::preferred_fruit_list>;
			}, variant);
		});

	std::cout << "at first, everyone prefers these fruits: " << everyone_prefers_these_fruits << std::endl;
	

	static constexpr auto entity_prefers_fruit = [] <typename Entity>(auto&& variant) {
			return std::visit([] (auto fruit) {
				return typewise::BelongsTo<decltype(fruit), typename Entity::preferred_fruit_list>;
			}, variant);
		};

	fruit_array[0] = grape{ size_type::big };		// Tom doesn't prefer grape
	fruit_array[1] = blueberry{ size_type::medium };	// the supermarket doesn't prefer blueberry
	fruit_array[2] = apple{ size_type::small };		// the fruit stand doesn't prefer apple

	bool not_everyone_prefers_their_distributed_fruit =
		rangewise<not_every, zip_pair_truncation>
		::between<entity_list>(fruit_array)
		.satisfies(entity_prefers_fruit);

	std::cout << "then, not everyone prefers their distributed fruit: "
		<< not_everyone_prefers_their_distributed_fruit << std::endl;


	swap(fruit_array[0], fruit_array[1]);
	swap(fruit_array[1], fruit_array[2]);

	// all_fruit_types::head{} is a placeholder and won't be used for padding in this scenario,
	// because fruit_array is longer
	bool everyone_prefers_their_distributed_fruit_and_the_fruit_company_takes_the_rest =
		rangewise<all_of, zip_pair_padding>
		::with_padding<fruit_company>(all_fruit_types::head{})
		.between<entity_list>(fruit_array)
		.satisfies(entity_prefers_fruit);

	std::cout << "finally, everyone prefers their distributed fruit and the fruit company takes the rest: "
		<< everyone_prefers_their_distributed_fruit_and_the_fruit_company_takes_the_rest << std::endl;

	// value wrapper and runtime array-like container --------------------------------------------------------------------------------

	std::cout << std::endl;

	static constexpr auto red_fruits_should_be_small = [] (auto fruit) {
			if constexpr (fruit.color == color_type::red) { return fruit.size == size_type::small; }
			return true;
		};

	static constexpr auto yellow_fruits_should_not_be_small = [] (auto fruit) {
			if constexpr (fruit.color == color_type::yellow) { return fruit.size != size_type::small; }
			return true;
		};

	static constexpr auto watermelons_should_be_big = [] (auto fruit) {
			if constexpr (std::same_as<decltype(fruit), watermelon>) { return fruit.size == size_type::big; }
			return true;
		};

	using overall_requirement_list = value_list<
		red_fruits_should_be_small,
		yellow_fruits_should_not_be_small,
		watermelons_should_be_big
	>;

	bool overall_requirements_complied =
		rangewise<all_of, cartesian_pair>
		::between<overall_requirement_list>(fruit_array)
		.satisfies([] <auto Requirement>(auto&& variant) { return std::visit(Requirement, variant); });

	std::cout << "overall requirements complied: " << overall_requirements_complied << std::endl;
	

	static constexpr auto the_first_fruit_should_be_blueberry = [] (auto fruit) {
			return std::same_as<decltype(fruit), blueberry>;
		};

	static constexpr auto the_fourth_fruit_should_be_medium = [] (auto fruit) {
			return fruit.size == size_type::medium;
		};

	static constexpr auto the_rest_should_not_be_cyan = [] (auto fruit) {
			return fruit.color != color_type::cyan;
		};

	static constexpr auto placeholder_requirement = [] (auto) { return true; };

	using orderwise_requirement_list = value_list<
		the_first_fruit_should_be_blueberry,
		placeholder_requirement,
		placeholder_requirement,
		the_fourth_fruit_should_be_medium
	>;

	// all_fruit_types::head{} is a placeholder and won't be used for padding in this scenario,
	// because fruit_array is longer
	bool orderwise_requirements_complied =
		rangewise<all_of, zip_pair_padding>
		::with_padding<the_rest_should_not_be_cyan>(all_fruit_types::head{})
		.between<orderwise_requirement_list>(fruit_array)
		.satisfies([] <auto Requirement>(auto&& variant) { return std::visit(Requirement, variant); });

	std::cout << "orderwise requirements complied: " << orderwise_requirements_complied << std::endl;

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

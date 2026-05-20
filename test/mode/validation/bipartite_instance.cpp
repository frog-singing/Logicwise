// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <array> // std::array
#include <vector> // std::vector
#include <ranges> // std::ranges

#include <algorithm> //用于 std::max
#include <iostream> // std::cout, std::boolalpha, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	struct combat_attribute
	{
		int attack{};
		int defense{};
		int health{};
	};

	//a >= 0, b > 0
	static constexpr int ceiling_division(int a, int b) noexcept { return (a + b - 1) / b; }

	static constexpr auto adventurer_beats_monsters = [] (auto&& adventurer, auto&& monsters)
		{
			int adventurer_health{ adventurer.health };
			if (adventurer_health <= 0) { return false; }

			int monster_quantity{ static_cast<int>(std::ranges::size(monsters)) };
			int defeated_monster_count{ 0 };

			while (true)
			{
				if (defeated_monster_count == monster_quantity) { return true; }

				int current_monster_health{ monsters[defeated_monster_count].health };

				while (current_monster_health > 0)
				{
					current_monster_health -= (std::max)(1, (adventurer.attack - monsters[defeated_monster_count].defense));

					for (int i = defeated_monster_count; i < monster_quantity; ++i)
					{
						adventurer_health -= (std::max)(1, (monsters[i].attack - adventurer.defense));
					}

					if (adventurer_health <= 0) { return false; }
				}

				++defeated_monster_count;
			}
		};

	static constexpr auto adventurer_beats_adventurer = [] (auto&& adventurer1, auto&& adventurer2)
		{
			int adventurer_1_strike_count = ceiling_division(
				adventurer2.health,
				(std::max)(1, (adventurer1.attack - adventurer2.defense))
			);

			int adventurer_2_strike_count = ceiling_division(
				adventurer1.health,
				(std::max)(1, (adventurer2.attack - adventurer1.defense))
			);

			return adventurer_1_strike_count < adventurer_2_strike_count;
		};

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h

	using namespace logicwise::test;

	// compile time vector-like container --------------------------------------------------------------------------------

	static constexpr combat_attribute
		warrior		{	.attack = 14,	.defense = 10,	.health = 108	},
		mage		{	.attack = 19,	.defense = 8,	.health = 75	},
		rogue		{	.attack = 23,	.defense = 5,	.health = 72	};

	static constexpr std::array adventurer_template_array{ warrior, mage, rogue };

	static constexpr combat_attribute
		slime		{	.attack = 12,	.defense = 8,	.health = 30	},
		zombie		{	.attack = 14,	.defense = 4,	.health = 40	},
		scorpion	{	.attack = 16,	.defense = 9,	.health = 22	},
		wolf		{	.attack = 18,	.defense = 6,	.health = 26	};

	static constexpr std::array monster_template_array{ slime, zombie, scorpion, wolf };

	static_assert(
		rangewise<all_of, cartesian_pair>
		::between(adventurer_template_array, monster_template_array)
		.satisfies([] (auto&& adventurer, auto&& monster) {

			if (adventurer.attack <= monster.defense || monster.attack <= adventurer.defense) { return false; }

			int adventurer_strike_count = ceiling_division(monster.health, (adventurer.attack - monster.defense));
			if (adventurer_strike_count <= 1 || adventurer_strike_count > 5) { return false; }

			int monster_strike_count = ceiling_division(adventurer.health, (monster.attack - adventurer.defense));
			if (monster_strike_count <= 3 || monster_strike_count > 100) { return false; }

			return monster_strike_count - adventurer_strike_count >= 2;
		}),
		"compile time container: adventurer-monster balance"
	);

	static_assert(
		rangewise<all_of, zip_pair_truncation>
		::between(adventurer_template_array, adventurer_template_array)
		.satisfies([] (auto&& adventurer, auto&& same_class_adventurer) {

			if (adventurer.attack <= same_class_adventurer.defense) { return false; }

			int strike_count = ceiling_division(
				same_class_adventurer.health,
				(adventurer.attack - same_class_adventurer.defense)
			);

			if (strike_count <= 3 || strike_count > 30) { return false; }

			return true;
		}),
		"compile time container: same class adventurer balance"
	);

	static_assert(
		rangewise<all_of, circular_adjacent_pair>
		::in(adventurer_template_array)
		.satisfies([] (auto&& adventurer1, auto&& adventurer2) {
			return adventurer_beats_adventurer(adventurer2, adventurer1);
		}),
		"compile time container: different class adventurer balance"
	);

	// compile time vector-like container and runtime vector-like container --------------------------------------------------------------------------------

	std::cout << std::boolalpha;

	static constexpr std::array<combat_attribute, 2>
		monster_pair_1	{	slime,		slime		},
		monster_pair_2	{	zombie,		zombie		},
		monster_pair_3	{	scorpion,	scorpion	},
		monster_pair_4	{	wolf,		wolf		};

	static constexpr std::array dungeon_monster_pairs{
		monster_pair_1,
		monster_pair_2,
		monster_pair_3
	};

	std::vector<combat_attribute> adventurer_team_A{};

	adventurer_team_A.push_back(warrior);
	adventurer_team_A.push_back(mage);
	adventurer_team_A.push_back(rogue);
	adventurer_team_A.push_back(warrior);
	adventurer_team_A.push_back(mage);

	bool adventurer_team_A_clears_dungeon_X =
		rangewise<all_of, zip_pair_truncation>
		::between(dungeon_monster_pairs, adventurer_team_A) // complie time and runtime
		.satisfies([] (auto&& monsters, auto&& adventurer) { return adventurer_beats_monsters(adventurer, monsters); });

	std::cout << "adventurer team A clears dungeon X: " << adventurer_team_A_clears_dungeon_X << std::endl;

	// combat_attribute{} is a placeholder and won't be used for padding in this scenario,
	// because adventurer_team_A is longer
	bool adventurer_team_A_clears_dungeon_Y =
		rangewise<all_of, zip_pair_padding>
		::with_padding(combat_attribute{}, monster_pair_4)
		.between(adventurer_team_A, dungeon_monster_pairs) // runtime and complie time
		.satisfies(adventurer_beats_monsters);

	std::cout << "adventurer team A clears dungeon Y: " << adventurer_team_A_clears_dungeon_Y << std::endl;

	// runtime vector-like container --------------------------------------------------------------------------------
	
	std::cout << std::endl;

	std::vector<combat_attribute> adventurer_team_B{ rogue, rogue, rogue };

	for (auto& adventurer : adventurer_team_B)
	{
		adventurer = warrior;
	}

	bool adventurer_team_A_wins_once_in_the_first_battle =
		rangewise<exactly<1>, zip_pair_truncation>
		::between(adventurer_team_A, adventurer_team_B)
		.satisfies(adventurer_beats_adventurer);

	std::cout << "adventurer team A wins once in the first battle: "
		<< adventurer_team_A_wins_once_in_the_first_battle << std::endl;

	// only warrior will be used for padding since adventurer_team_B is shorter
	bool adventurer_team_A_wins_twice_in_the_second_battle =
		rangewise<exactly<2>, zip_pair_padding>
		::with_padding(rogue, warrior)
		.between(adventurer_team_A, adventurer_team_B)
		.satisfies(adventurer_beats_adventurer);

	std::cout << "adventurer team A wins twice in the second battle: "
		<< adventurer_team_A_wins_twice_in_the_second_battle << std::endl;

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

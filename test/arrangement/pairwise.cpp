// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <iostream> // std::cout, std::endl


namespace logicwise::test
{
	template<typename index_sequence>
	static void print()
	{
		if constexpr (wrapper::as_value_list<index_sequence>::empty)
		{
			std::cout << "empty index sequence" << std::endl;
		}
		else
		{
			using index_type = detail::IndexTrait2D::index_type;

			index_sequence::invoke([] <index_type Head, index_type... Tail> {
				std::cout << "(" << Head[0] << ", " << Head[1] << ")";
				(..., (std::cout << ", (" << Tail[0] << ", " << Tail[1] << ")"));
				std::cout << std::endl;
			});
		}
	}

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::detail; // for Extent1D and index_sequencer

	using namespace logicwise::test;

	//--------------------------------------------------------------------------------

	static constexpr Extent1D extent{ 4 };
	// try to replace extent with other non-negative numbers

	using forward_index_sequence_for_permutation_pair = index_sequencer<
		arrangement::permutation_pair,
		arrangement::permutation_pair::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_permutation_pair = index_sequencer<
		arrangement::permutation_pair,
		arrangement::permutation_pair::reverse_index_traverser,
		extent
	>::index_sequence;

	using forward_index_sequence_for_combination_pair = index_sequencer<
		arrangement::combination_pair,
		arrangement::combination_pair::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_combination_pair = index_sequencer<
		arrangement::combination_pair,
		arrangement::combination_pair::reverse_index_traverser,
		extent
	>::index_sequence;

	using forward_index_sequence_for_linear_adjacent_pair = index_sequencer<
		arrangement::linear_adjacent_pair,
		arrangement::linear_adjacent_pair::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_linear_adjacent_pair = index_sequencer<
		arrangement::linear_adjacent_pair,
		arrangement::linear_adjacent_pair::reverse_index_traverser,
		extent
	>::index_sequence;

	using forward_index_sequence_for_circular_adjacent_pair = index_sequencer<
		arrangement::circular_adjacent_pair,
		arrangement::circular_adjacent_pair::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_circular_adjacent_pair = index_sequencer<
		arrangement::circular_adjacent_pair,
		arrangement::circular_adjacent_pair::reverse_index_traverser,
		extent
	>::index_sequence;

	//--------------------------------------------------------------------------------

	std::cout << "Extent: " << extent.i() << std::endl;

	std::cout << std::endl << "Forward index sequence for permutation pair: " << std::endl;
	print<forward_index_sequence_for_permutation_pair>();

	std::cout << std::endl << "Reverse index sequence for permutation pair: " << std::endl;
	print<reverse_index_sequence_for_permutation_pair>();

	std::cout << std::endl << "Forward index sequence for combination pair: " << std::endl;
	print<forward_index_sequence_for_combination_pair>();

	std::cout << std::endl << "Reverse index sequence for combination pair: " << std::endl;
	print<reverse_index_sequence_for_combination_pair>();

	std::cout << std::endl << "Forward index sequence for linear adjacent pair: " << std::endl;
	print<forward_index_sequence_for_linear_adjacent_pair>();

	std::cout << std::endl << "Reverse index sequence for linear adjacent pair: " << std::endl;
	print<reverse_index_sequence_for_linear_adjacent_pair>();

	std::cout << std::endl << "Forward index sequence for circular adjacent pair: " << std::endl;
	print<forward_index_sequence_for_circular_adjacent_pair>();

	std::cout << std::endl << "Reverse index sequence for circular adjacent pair: " << std::endl;
	print<reverse_index_sequence_for_circular_adjacent_pair>();

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

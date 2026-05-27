// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <iostream> // std::cout, std::boolalpha, std::endl


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
			using index_type = detail::IndexTraitPadding2D::index_type;

			index_sequence::invoke([] <index_type Head, index_type... Tail> {
				std::cout << "( (" << Head.component[0] << ", " << Head.component[1] << "), ("
					<< Head.padding_state[0] << ", " << Head.padding_state[1] << ") )";
				
				(..., (std::cout << ",\n( (" << Tail.component[0] << ", " << Tail.component[1] << "), ("
					<< Tail.padding_state[0] << ", " << Tail.padding_state[1] << ") )"));
				
				std::cout << std::endl;
			});
		}
	}

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::detail; // for Extent2D and index_sequencer

	using namespace logicwise::test;

	//--------------------------------------------------------------------------------
	
	static constexpr Extent2D extent{ 3, 5 };
	// try to replace extent with other non-negative integer pairs

	using forward_index_sequence_for_zip_pair_padding = index_sequencer<
		arrangement::zip_pair_padding,
		arrangement::zip_pair_padding::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_zip_pair_padding = index_sequencer<
		arrangement::zip_pair_padding,
		arrangement::zip_pair_padding::reverse_index_traverser,
		extent
	>::index_sequence;

	//--------------------------------------------------------------------------------

	std::cout << std::boolalpha;

	std::cout << "Extent: (" << extent.i() << ", " << extent.j() << ")" << std::endl;

	std::cout << std::endl << "Forward index sequence for zip pair padding: " << std::endl;
	print<forward_index_sequence_for_zip_pair_padding>();

	std::cout << std::endl << "Reverse index sequence for zip pair padding: " << std::endl;
	print<reverse_index_sequence_for_zip_pair_padding>();

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

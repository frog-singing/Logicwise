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
	using namespace logicwise::detail; // for Extent2D and index_sequencer

	using namespace logicwise::test;

	//--------------------------------------------------------------------------------

	static constexpr Extent2D extent{ 3, 4 };
	// try to replace extent with other non-negative number pairs

	using forward_index_sequence_for_cartesian_pair = index_sequencer<
		arrangement::cartesian_pair,
		arrangement::cartesian_pair::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_cartesian_pair = index_sequencer<
		arrangement::cartesian_pair,
		arrangement::cartesian_pair::reverse_index_traverser,
		extent
	>::index_sequence;

	using forward_index_sequence_for_zip_pair_truncation = index_sequencer<
		arrangement::zip_pair_truncation,
		arrangement::zip_pair_truncation::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_zip_pair_truncation = index_sequencer<
		arrangement::zip_pair_truncation,
		arrangement::zip_pair_truncation::reverse_index_traverser,
		extent
	>::index_sequence;

	//--------------------------------------------------------------------------------

	std::cout << "Extent: (" << extent.i() << ", " << extent.j() << ")" << std::endl;

	std::cout << std::endl << "Forward index sequence for cartesian pair: " << std::endl;
	print<forward_index_sequence_for_cartesian_pair>();

	std::cout << std::endl << "Reverse index sequence for cartesian pair: " << std::endl;
	print<reverse_index_sequence_for_cartesian_pair>();

	std::cout << std::endl << "Forward index sequence for zip pair truncation: " << std::endl;
	print<forward_index_sequence_for_zip_pair_truncation>();

	std::cout << std::endl << "Reverse index sequence for zip pair truncation: " << std::endl;
	print<reverse_index_sequence_for_zip_pair_truncation>();

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

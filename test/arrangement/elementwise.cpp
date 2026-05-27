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
			using index_type = detail::IndexTrait1D::index_type;

			index_sequence::invoke([] <index_type Head, index_type... Tail> {
				std::cout << Head;
				(..., (std::cout << ", " << Tail));
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

	static constexpr Extent1D extent{ 5 };
	// try to replace extent with other non-negative integers

	using forward_index_sequence_for_element = index_sequencer<
		arrangement::element,
		arrangement::element::forward_index_traverser,
		extent
	>::index_sequence;

	using reverse_index_sequence_for_element = index_sequencer<
		arrangement::element,
		arrangement::element::reverse_index_traverser,
		extent
	>::index_sequence;

	//--------------------------------------------------------------------------------

	std::cout << "Extent: " << extent.i() << std::endl;

	std::cout << std::endl << "Forward index sequence for element: " << std::endl;
	print<forward_index_sequence_for_element>();

	std::cout << std::endl << "Reverse index sequence for element: " << std::endl;
	print<reverse_index_sequence_for_element>();

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

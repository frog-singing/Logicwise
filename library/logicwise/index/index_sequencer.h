// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array
#include <utility> //用于 std::index_sequence, std::make_index_sequence


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	template<typename Arrangement, typename IndexTraverserType, typename Arrangement::extent_type Extent>
	struct index_sequencer
	{
		static constexpr std::size_t index_count{ Arrangement::index_count(Extent) };

		using index_trait = typename Arrangement::index_trait;
		using index_type = typename index_trait::index_type;
		using index_array_type = std::array<index_type, index_count>;

		static constexpr auto generate_index_array()
		{
			index_array_type index_array{};

			IndexTraverserType index_traverser{ Extent };
			std::size_t i{ 0 };

			while (!index_traverser.done())
			{
				//[[assume(i < index_count)]]; //C++23标准

				index_array[i] = index_traverser.state();
				++i;
				index_traverser.step();
			}

			return index_array;
		}

		template<auto IndexArray = generate_index_array(), std::size_t... I>
		static consteval auto print_index_sequence(std::index_sequence<I...>)
		{
			return typename index_trait::template index_sequence<IndexArray[I]...>{};
		}

		using index_sequence = decltype
		(
			print_index_sequence(std::make_index_sequence<index_count>{})
		);

	};

}

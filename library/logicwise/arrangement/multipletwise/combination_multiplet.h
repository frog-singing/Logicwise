// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/detail/safe_integer_cast.h>
#include <logicwise/arrangement/voidwise/void.h>
#include <logicwise/arrangement/elementwise/element.h>
#include <logicwise/arrangement/pairwise/circular_adjacent_pair.h>
#include <cstddef> //用于 std::size_t
#include <algorithm> //用于 std::min, std::max
#include <cassert> //用于 assert
#include <concepts> //用于 std::integral，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::逐多元组 arrangement::multipletwise================================================================================

	//保序组合多元组
	template<std::size_t Arity>
	struct combination_multiplet_impl : multipletwise_arrangement_tag
	{
		using type = combination_multiplet_impl;

		using extent_type = Extent1D;
		using index_trait = IndexTrait<Arity>;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;

		static constexpr auto SampleSize{ safe_integer_cast<index_integer_type>(Arity) };

        static constexpr std::size_t index_count(extent_type extent) noexcept
        {
            const std::size_t extent_i{ extent.i() };
            if (extent_i < Arity) { return 0; }

            const std::size_t complement_size{ extent_i - Arity };
			const std::size_t sample_size   { (std::min)(Arity, complement_size) };
			const std::size_t base_offset   { (std::max)(Arity, complement_size) };

            std::size_t count{ 1 };

            for (std::size_t i{ 1 }; i <= sample_size; ++i)
            {
                count = count * (base_offset + i) / i;
            }

            return count;
        }

		struct forward_index_traverser
        {
            const index_integer_type max_index;
            index_integer_type index_tail{};
            index_type index{};

            forward_index_traverser() = delete; //禁用默认构造函数

            explicit constexpr forward_index_traverser(extent_type extent) noexcept
                : max_index{ safe_integer_cast<index_integer_type>(extent.i()) - 1 }
            {
				//Arity 为 0 或溢出
                if constexpr (SampleSize == 0)
                {
                    index_tail = max_index + 1;
                    return;
                }

				index_tail = SampleSize - 1;

                for (std::size_t i{ 0 }; i < Arity; ++i)
                {
                    index[i] = i;
                }
            }

            constexpr bool done() const noexcept { return index_tail > max_index; }

            constexpr const index_type& state() const noexcept
            {
                assert(!done() && index_tail >= 0 && "[logicwise] Accessing illegal index.");
                
                return index;
            }

            //Arity >= 2 且未溢出
			constexpr void step() noexcept requires (SampleSize >= 2)
            {
				//不要用 index_tail < max_index，否则处于非法索引时 step 会导致非法状态不能被 if 快速捕获，导致无效开销
				if (index_tail != max_index)
				{
					++index_tail;
					++index[Arity - 1];
					return;
				}

                std::size_t position{ Arity - 2 };

                while (position > 0 && index[position] + 1 == index[position + 1])
                {
                    --position;
                }

                std::size_t index_current{ index[position] };

                for (std::size_t i{ position }; i < Arity; ++i)
                {
                    ++index_current;
					index[i] = index_current;
                }

				index_tail = static_cast<index_integer_type>(index[Arity - 1]);
            }
        };

        struct reverse_index_traverser
        {
            const std::size_t max_index;
            index_integer_type index_head{};
            index_type index{};

            reverse_index_traverser() = delete; //禁用默认构造函数

            explicit constexpr reverse_index_traverser(extent_type extent) noexcept
                : max_index{ extent.i() - 1 }
            {
                //Arity 为 0 或溢出
                if constexpr (SampleSize == 0)
                {
                    index_head = -1;
                    return;
                }

                index_head = safe_integer_cast<index_integer_type>(extent.i()) - SampleSize;

                std::size_t index_current{ static_cast<std::size_t>(index_head) };

                for (std::size_t i{ 0 }; i < Arity; ++i)
                {
                    index[i] = index_current;
                    ++index_current;
                }
            }

            constexpr bool done() const noexcept { return index_head < 0; }

            constexpr const index_type& state() const noexcept
            {
                assert(!done() && "[logicwise] Accessing illegal index.");

                return index;
            }

            //Arity >= 1 且未溢出
            constexpr void step() noexcept requires (SampleSize >= 1)
            {
                std::size_t position{ Arity - 1 };

                while (position > 0 && index[position - 1] == index[position] - 1)
                {
                    --position;
                }

                if (position == 0) { --index_head; }

                --index[position];

                std::size_t index_current{ max_index };

                for (std::size_t i{ Arity - 1 }; i > position; --i)
                {
                    index[i] = index_current;
					--index_current;
                }
            }
        };

        using fast_index_traverser = forward_index_traverser;
        using light_index_traverser = forward_index_traverser;

	};
	
	template<> struct combination_multiplet_impl<0> { using type = multipletwise_void; };
	template<> struct combination_multiplet_impl<1> { using type = multipletwise_element; };
	template<> struct combination_multiplet_impl<2> { using type = combination_pair; };

	template<std::integral auto Arity> requires (Arity >= 0)
	using combination_multiplet = typename combination_multiplet_impl<static_cast<std::size_t>(Arity)>::type;

}

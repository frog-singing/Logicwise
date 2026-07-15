// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/semantics/guard/safe_integer_cast.h>
#include <cstddef> //用于 std::size_t
#include <cassert> //用于 assert


namespace logicwise::detail
{
    template<typename IndexTrait>
    struct base_element;

	struct element;
	struct multipletwise_element;
	struct multipartite_element;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::逐元素 arrangement::elementwise================================================================================

	//基础元素
    template<typename IndexTrait>
    struct base_element
	{
		using extent_type = Extent1D;
        using index_trait = IndexTrait;
		using index_type = typename index_trait::index_type;
        using index_integer_type = int;

        static constexpr std::size_t index_count(extent_type extent) noexcept
        {
            return extent.i();
        }

        struct forward_index_traverser
        {
            const index_integer_type extent_i;
            index_integer_type index_i{ 0 };

            forward_index_traverser() = delete; //禁用默认构造函数

            explicit constexpr forward_index_traverser(extent_type extent) noexcept
                : extent_i{ safe_integer_cast<index_integer_type>(extent.i()) }
            {}

            constexpr bool done() const noexcept { return index_i >= extent_i; }

            constexpr auto state() const noexcept
            {
                assert(!done() && index_i >= 0 && "[logicwise] Accessing illegal index.");

                return index_type
                {
                    static_cast<std::size_t>(index_i)
                };
            }

            constexpr void step() noexcept { ++index_i; }
        };

        struct reverse_index_traverser
        {
            index_integer_type index_i;

            reverse_index_traverser() = delete; //禁用默认构造函数

            explicit constexpr reverse_index_traverser(extent_type extent) noexcept
                : index_i{ safe_integer_cast<index_integer_type>(extent.i()) - 1 }
            {}

            constexpr bool done() const noexcept { return index_i < 0; }

            constexpr auto state() const noexcept
            {
                assert(!done() && "[logicwise] Accessing illegal index.");

                return index_type
                {
                    static_cast<std::size_t>(index_i)
                };
            }

            constexpr void step() noexcept { --index_i; }
        };

        using fast_index_traverser = reverse_index_traverser;
        using light_index_traverser = reverse_index_traverser;

	};

    //单个元素
    struct element : base_element<IndexTraitScalar>, elementwise_arrangement_tag {};

	//逐多元组元素
	struct multipletwise_element : base_element<IndexTrait1D>, multipletwise_arrangement_tag {};

    //多部元素
	struct multipartite_element : base_element<IndexTrait1D>, multipartite_arrangement_tag {};
}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/vector_like.h>
#include <type_traits> //用于 std::is_lvalue_reference_v, std::remove_cvref_t, std::remove_cv_t,
//std::conditional_t, std::is_array_v, std::remove_reference_t, std::remove_extent_t, std::extent_v
#include <ranges> //用于 std::ranges，C++20标准
#include <cstddef> //用于 std::size_t
#include <span> //用于 std::span
#include <array> //用于 std::array, std::to_array
#include <concepts> //用于 std::move_constructible, std::convertible_to, std::derived_from, std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
    //细节 detail================================================================================

    template<VectorLike ContainerType>
    struct vector_like_container_trait
    {
        static constexpr bool is_left_value = std::is_lvalue_reference_v<ContainerType>;

        //去掉引用和 const volatile
        using raw_container_type = std::remove_cvref_t<ContainerType>;

        static constexpr bool should_store_as_reference = is_left_value && !std::ranges::view<raw_container_type>;

        //接口
        using stored_instance_type = std::ranges::range_value_t<raw_container_type>;

        //接口
        using raw_instance_type = std::remove_cv_t<stored_instance_type>;

        //接口
        using stored_container_type = std::conditional_t<
            should_store_as_reference,
            const raw_container_type&,
            raw_container_type
        >;

        //接口
        using expected_container_type = std::conditional_t<
            should_store_as_reference,
            const raw_container_type&,
            ContainerType
        >;

        //接口
        static constexpr stored_container_type cast_container(expected_container_type container) noexcept
        {
            return static_cast<expected_container_type>(container);
        }

    };

    template<VectorLike ContainerType>
        requires std::is_array_v<std::remove_cvref_t<ContainerType>>
    struct vector_like_container_trait<ContainerType>
    {
        static constexpr bool is_left_value = std::is_lvalue_reference_v<ContainerType>;

        //去掉引用
        using direct_container_type = std::remove_reference_t<ContainerType>;

        //去掉引用和 const volatile
        using raw_container_type = std::remove_cvref_t<ContainerType>;

        using direct_instance_type = std::remove_extent_t<direct_container_type>;

        //接口
        using raw_instance_type = std::remove_extent_t<raw_container_type>;

        //接口
        using stored_instance_type = std::conditional_t<
            is_left_value,
            direct_instance_type,
            raw_instance_type
        >;

        static constexpr std::size_t extent = std::extent_v<raw_container_type>;

        //接口
        using stored_container_type = std::conditional_t<
            is_left_value,
            std::span<direct_instance_type, extent>,
            std::array<raw_instance_type, extent>
        >;

        //接口
        using expected_container_type = std::conditional_t<
            is_left_value,
            direct_instance_type(&)[extent],
            direct_instance_type(&&)[extent]
        >;

        //接口
        static constexpr stored_container_type cast_container(expected_container_type container) noexcept
        {
            if constexpr (is_left_value)
            {
                return std::span{ container };
            }
            else
            {
                static_assert(!std::is_array_v<raw_instance_type>,
                    "[logicwise] Error: Multidimensional arrays (T[N][M]) cannot be stored by value.\n"
                    "Consider using reference or std::array<std::array<T, M>, N> instead.");

                static_assert(std::move_constructible<raw_instance_type>,
                    "[logicwise] Error: Array element must be move-constructible to be converted to std::array.");

                return std::to_array(static_cast<expected_container_type>(container));
            }
        }

    };

    //--------------------------------------------------------------------------------

    template<typename PaddingInstanceType>
    struct padding_instance_trait
    {
        static constexpr bool is_left_value = std::is_lvalue_reference_v<PaddingInstanceType>;

        //去掉引用和 const volatile
        using raw_padding_instance_type = std::remove_cvref_t<PaddingInstanceType>;

        //接口
        using stored_padding_instance_type = std::conditional_t <
            is_left_value,
            const raw_padding_instance_type&,
            raw_padding_instance_type
        >;

        //接口
        //convertible_to 禁止显式转换场景下的隐式转换
        template<typename TargetRawType>
        static constexpr bool is_compatible_type = 
			std::convertible_to<stored_padding_instance_type, TargetRawType>;

        template<typename TargetRawType>
        static constexpr bool should_normalize_as_reference = is_left_value &&
            (
                std::derived_from<raw_padding_instance_type, TargetRawType> ||
                std::same_as<raw_padding_instance_type, TargetRawType>
            );

        //接口
		//需要满足 is_compatible_type<TargetRawType>
        template<typename TargetRawType>
        using normalized_padding_instance_type = std::conditional_t<
            should_normalize_as_reference<TargetRawType>,
            const TargetRawType&,
            TargetRawType
        >;

    };

}

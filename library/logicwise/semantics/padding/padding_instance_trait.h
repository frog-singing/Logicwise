// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <type_traits> //用于 std::is_lvalue_reference_v, std::remove_cvref_t (C++20), std::conditional_t
#include <concepts> //用于 std::convertible_to, std::derived_from, std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
    //语义::填充 semantics::padding================================================================================

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

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::integral，C++20标准
#include <utility> //用于 std::cmp_greater (C++20), std::cmp_less (C++20)
#include <limits> //用于 std::numeric_limits
#include <cassert> //用于 assert


//逻辑维度::细节
namespace logicwise::detail
{
    //细节 detail================================================================================

    template<std::integral Target, std::integral Source>
    [[nodiscard]] constexpr Target safe_integer_cast(Source value) noexcept
    {
        if (std::cmp_greater(value, std::numeric_limits<Target>::max()) ||
            std::cmp_less(value, std::numeric_limits<Target>::min()))
        {
            assert(false && "[logicwise] Narrowing conversion : integer cast overflow.");
            return 0;
        }
        return static_cast<Target>(value);
    }

}

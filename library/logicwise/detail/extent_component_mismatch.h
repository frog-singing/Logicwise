// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <cassert> //用于 assert
#include <exception> //用于 std::terminate


//逻辑维度::细节
namespace logicwise::detail
{
    //细节 detail================================================================================

    [[noreturn]] inline void extent_component_mismatch() noexcept
    {
        assert(false && "[logicwise] Extent component mismatch.");
        std::terminate();
    }

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cassert> //用于 assert
#include <exception> //用于 std::terminate


//逻辑维度::细节
namespace logicwise::detail
{
    //细节 detail================================================================================

    [[noreturn]] inline void cycle_detected_in_graph() noexcept
    {
        assert(false && "[logicwise] Cycle detected in graph.");
        std::terminate();
    }

}

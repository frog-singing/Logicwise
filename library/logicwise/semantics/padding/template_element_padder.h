// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cstddef> //用于 std::size_t


//逻辑维度::细节
namespace logicwise::detail
{
    //语义::填充 semantics::padding================================================================================

    struct template_element_padder
    {
        //类型维度填充
        template<bool ShouldPad, typename PaddingElement, typename TypeList, std::size_t ElementIndex>
        struct typewise_padding
        {
			using element = PaddingElement;
        };

        template<typename PaddingElement, typename TypeList, std::size_t ElementIndex>
        struct typewise_padding<false, PaddingElement, TypeList, ElementIndex>
        {
			using element = typename TypeList::template element<ElementIndex>;
        };

        template<bool ShouldPad, typename PaddingElement, typename TypeList, std::size_t ElementIndex>
		using actual_type = typename typewise_padding<ShouldPad, PaddingElement, TypeList, ElementIndex>::element;

		//值维度填充
		template<bool ShouldPad, auto PaddingElement, typename ValueList, std::size_t ElementIndex>
        struct valuewise_padding
        {
			static constexpr auto element = PaddingElement;
        };

        template<auto PaddingElement, typename ValueList, std::size_t ElementIndex>
        struct valuewise_padding<false, PaddingElement, ValueList, ElementIndex>
        {
			static constexpr auto element = ValueList::template element<ElementIndex>;
        };

        template<bool ShouldPad, auto PaddingElement, typename ValueList, std::size_t ElementIndex>
        static constexpr auto actual_value = valuewise_padding<ShouldPad, PaddingElement, ValueList, ElementIndex>::element;

    };

}

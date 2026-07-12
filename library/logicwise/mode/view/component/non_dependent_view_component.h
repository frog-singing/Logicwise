// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/view/adaptor/non_dependent_view_adaptor.h>
#include <cstddef> //用于 std::size_t


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename ViewPipeline>
	struct non_dependent_view_component
	{
		template<typename Adaptor>
		using new_view = ViewPipeline::template new_view<Adaptor>;

		struct component
		{
			static constexpr auto reverse()
			{
				return new_view< non_dependent_view_adaptor::reverse >{};
			}

			//--------------------------------------------------------------------------------

			template<std::size_t Count>
			static constexpr auto take()
			{
				return new_view< non_dependent_view_adaptor::take<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto drop()
			{
				return new_view< non_dependent_view_adaptor::drop<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto take_last()
			{
				return new_view< non_dependent_view_adaptor::take_last<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto drop_last()
			{
				return new_view< non_dependent_view_adaptor::drop_last<Count> >{};
			}

			template<std::size_t Start, std::size_t End>
			static constexpr auto slice()
			{
				return new_view< non_dependent_view_adaptor::slice<Start, End> >{};
			}

		};

	};

}

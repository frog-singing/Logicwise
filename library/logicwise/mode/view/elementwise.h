// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/type.h>
#include <logicwise/index/index_sequencer.h>
#include <logicwise/semantics/material/type_list/view_protocol.h>
#include <logicwise/semantics/material/value_list/view_protocol.h>
#include "concrete_view.h"


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class elementwise_viewing;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename Mode, typename Arrangement>
	class elementwise_viewing
	{
	public:
		template<typewise::List WrapperInstance>
		[[nodiscard]] static constexpr auto in()
		{
			using List = as_type_list<WrapperInstance>;

			return concrete_view<
					type_list_view_protocol<List>,
					initial_view_data<List>()
				>{};
		}

		template<valuewise::List WrapperInstance>
		[[nodiscard]] static constexpr auto in()
		{
			using List = as_value_list<WrapperInstance>;

			return concrete_view<
					value_list_view_protocol<List>,
					initial_view_data<List>()
				>{};
		}

		template<typename WrapperInstance>
		static constexpr auto in()
		{
			static_assert(dependent_false_v<WrapperInstance>,
				"[logicwise] Error: Incompatible <WrapperInstance>!\n"
				"Expected: pure type list or pure value list.");

			return false;
		}

		template<typename ContainerType>
		static constexpr auto in(ContainerType&&)
		{
			static_assert(dependent_false_v<ContainerType>,
				"[logicwise] Container view not supported.");

			return false;
		}


	private:
		using extent_type = typename Arrangement::extent_type;
		using IndexTraverserType = typename Mode::template index_traverser<Arrangement>;

		template<typename List>
		static constexpr auto initial_view_data()
		{
			constexpr extent_type Extent{ List::size };
			using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;
			constexpr auto IndexArray{ IndexSequencer::generate_index_array() };

			return ViewData{ IndexArray, 0, IndexArray.size() };
		}

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include "valid_extent.h"
#include <concepts> //用于 std::same_as, std::convertible_to，C++20标准
#include <cstddef> //用于 std::size_t
#include <type_traits> //用于 std::remove_cvref_t (C++20)
#include <tuple> //用于 std::tuple_size, std::tuple_size_v
#include <span> //用于 std::dynamic_extent


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	template<typename Arrangement, auto Extent>
	concept TraversableExtent =
		ValidExtent<Arrangement, Extent> &&
		!(typename Arrangement::light_index_traverser{ Extent }.done());

	//需要满足 TraversableExtent<Arrangement, Extent>
	//state() 的返回类型可能是 index_type 或 const index_type&，此处 probe_index 必须进行值拷贝，即 auto，而非 decltype(auto)
	template<typename Arrangement, auto Extent>
	inline constexpr auto probe_index
	{
		typename Arrangement::light_index_traverser{ Extent }.state()
	};

	template<typename ContainerType>
	inline constexpr std::size_t static_container_size = []{

		using RawType = std::remove_cvref_t<ContainerType>;

		if constexpr		( requires{	{ RawType::size						} -> std::convertible_to<std::size_t>;	}	)
							{ return	  RawType::size;																}

		else if constexpr	( requires{	{ RawType::size()					} -> std::convertible_to<std::size_t>;	}	)
							{ return	  RawType::size();																}
		//tuple-like
		else if constexpr	( requires{	{ std::tuple_size<RawType>::value	} -> std::convertible_to<std::size_t>;	}	)
							{ return	  std::tuple_size_v<RawType>;													}
		//span
		else if constexpr	( requires{	{ RawType::extent					} -> std::convertible_to<std::size_t>;	}
							  &&		  RawType::extent != std::dynamic_extent									  	)
							{ return	  RawType::extent;																}

		else if constexpr	( requires{	{ RawType{}.size()					} -> std::convertible_to<std::size_t>;	}	)
							{ return	  RawType{}.size();																}

		else
		{
			static_assert(dependent_false_v<RawType>,
				"[logicwise] Requires static size for container.");

			return 0;
		}
	}();

}

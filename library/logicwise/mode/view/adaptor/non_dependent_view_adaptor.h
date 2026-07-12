// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/index/type.h>
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	struct non_dependent_view_adaptor
	{
		struct identity
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				return Data;
			}
		};

		//--------------------------------------------------------------------------------

		struct reverse
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t NextSize{ Data.size };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }, j{ NextSize - 1 }; i < NextSize; ++i, --j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//================================================================================

		template<std::size_t Count>
		struct take
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }; i < NextSize; ++i)
				{
					next_index_array[i] = Data[i];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		struct drop
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }, j{ Count }; i < NextSize; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		struct take_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }, j{ MaxSize - NextSize }; i < NextSize; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		struct drop_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }; i < NextSize; ++i)
				{
					next_index_array[i] = Data[i];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------

		//左闭右开区间，[Start, End)
		//此实现等价于 drop<Start>().take<End - Start>() 或 drop<Offset>().take<Count>()
		//不等价于 take<End>().drop<Start>()
		template<std::size_t Start, std::size_t End>
		struct slice
		{
			static_assert(Start <= End, "[logicwise] Invalid slice range : Start must be less than or equal to End.");

			template<auto Data>
			static constexpr auto adapt()
			{
				using index_type = typename decltype(Data)::index_type;

				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t CurrentEnd{ Data.size };
				constexpr std::size_t ActualStart{ Start < CurrentEnd ? Start : CurrentEnd };
				constexpr std::size_t ActualEnd{ End < CurrentEnd ? End : CurrentEnd };
				constexpr std::size_t NextSize{ ActualEnd - ActualStart };

				std::array<index_type, NextSize> next_index_array{};

				for (std::size_t i{ 0 }, j{ ActualStart }; j < ActualEnd; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

	};

}

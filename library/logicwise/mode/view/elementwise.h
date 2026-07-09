// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>

#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/element.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/index/index_sequencer.h>
#include <logicwise/semantics/vector_like_container.h>
#include "view_loop.h"
#include "view_bake.h"
#include <utility> //用于 std::forward, std::index_sequence, std::make_index_sequence
#include <type_traits> //用于 std::remove_reference_t
#include <functional> //用于 std::invoke
#include <cstddef> //用于 std::size_t
#include <variant> //用于 std::monostate
#include <array> //用于 std::array
#include <ranges> //用于 std::ranges, std::views，C++20标准


namespace logicwise::detail
{
	struct non_dependent_view_adaptor;

	template<typename BoolSolver>
	struct view_adaptor;

	template<typename List>
	struct type_list_bool_solver;

	template<typename List>
	struct value_list_bool_solver;

	struct type_list_error_reporter;
	struct value_list_error_reporter;

	template<typename List>
	struct type_list_view_protocol;

	template<typename List>
	struct value_list_view_protocol;

	template<typename ViewProtocol, auto Data>
	struct concrete_view;

	template<typename Mode, typename Arrangement>
	class elementwise_viewing;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================
	
	struct non_dependent_view_adaptor
	{
		static constexpr auto identity = [] <auto Data> { return Data; };

		//--------------------------------------------------------------------------------

		static constexpr auto reverse = [] <auto Data> {

			using index_type = typename decltype(Data)::index_type;

			constexpr std::size_t NextOffset{ 0 };
			constexpr std::size_t NextSize{ Data.size };

			std::array<index_type, NextSize> next_index_array{};

			for (std::size_t i{ 0 }, j{ NextSize - 1 }; i < NextSize; ++i, --j)
			{
				next_index_array[i] = Data[j];
			}

			return ViewData{ next_index_array, NextOffset, NextSize };
		};

		//================================================================================

		template<std::size_t Count>
		static constexpr auto take = [] <auto Data> {

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
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		static constexpr auto drop = [] <auto Data> {

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
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		static constexpr auto take_last = [] <auto Data> {

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
		};

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		static constexpr auto drop_last = [] <auto Data> {

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
		};

		//--------------------------------------------------------------------------------

		//左闭右开区间，[Start, End)
		//此实现等价于 drop<Start>().take<End - Start>() 或 drop<Offset>().take<Count>()
		//不等价于 take<End>().drop<Start>()
		template<std::size_t Start, std::size_t End>
		static constexpr auto slice = [] <auto Data> {

			static_assert(Start <= End, "[logicwise] Invalid slice range : Start must be less than or equal to End.");

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
		};

	};


	template<typename BoolSolver>
	struct view_adaptor
	{
		using bool_solver = BoolSolver;
		using index_type = typename bool_solver::index_type;

		template<typename Predicate>
		using predicate_solver = typename bool_solver::template predicate_solver<Predicate>;

		template<typename Predicate>
		using predicate_with_index_solver = typename bool_solver::template predicate_with_index_solver<Predicate>;

		template<typename HomogeneousRelation>
		using homogeneous_relation_solver = typename bool_solver::template homogeneous_relation_solver<HomogeneousRelation>;

		//================================================================================

		template<typename Predicate>
		static constexpr auto take_while = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (!predicate_solver<Predicate>::template solve<Index>())
				{
					break;
				}
				next_index_array[next_size] = Index;
				++next_size;
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(... || [&] <auto Index> {
					if constexpr (!predicate_solver<Predicate>::template solve<Index>())
					{
						return true;
					}
					next_index_array[next_size] = Index;
					++next_size;
					return false;
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto take_until = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (predicate_solver<Predicate>::template solve<Index>())
				{
					break;
				}
				next_index_array[next_size] = Index;
				++next_size;
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(... || [&] <auto Index> {
					if constexpr (predicate_solver<Predicate>::template solve<Index>())
					{
						return true;
					}
					next_index_array[next_size] = Index;
					++next_size;
					return false;
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto drop_while = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			constexpr std::size_t NextSize = []{

				std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (!predicate_solver<Predicate>::template solve<Index>())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (!predicate_solver<Predicate>::template solve<Index>())
						{
							return true;
						}
						--next_size;
						return false;
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return next_size;
			}();

			std::array<index_type, NextSize> next_index_array{};

			for (std::size_t i{ 0 }, j{ Data.size - NextSize }; i < NextSize; ++i, ++j)
			{
				next_index_array[i] = Data[j];
			}

			return ViewData{ next_index_array, NextOffset, NextSize };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto drop_until = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			constexpr std::size_t NextSize = []{

				std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (predicate_solver<Predicate>::template solve<Index>())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (predicate_solver<Predicate>::template solve<Index>())
						{
							return true;
						}
						--next_size;
						return false;
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return next_size;
			}();

			std::array<index_type, NextSize> next_index_array{};

			for (std::size_t i{ 0 }, j{ Data.size - NextSize }; i < NextSize; ++i, ++j)
			{
				next_index_array[i] = Data[j];
			}

			return ViewData{ next_index_array, NextOffset, NextSize };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto take_last_while = [] <auto Data> {

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_offset{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data | std::views::reverse)
			{
				if constexpr (!predicate_solver<Predicate>::template solve<Index>())
				{
					break;
				}
				--next_offset;
				next_index_array[next_offset] = Index;
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(... || [&] <auto Index> {
					if constexpr (!predicate_solver<Predicate>::template solve<Index>())
					{
						return true;
					}
					--next_offset;
					next_index_array[next_offset] = Index;
					return false;
				}.template operator() < Data[Data.size - I - 1] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			std::size_t next_size{ Data.size - next_offset };

			return ViewData{ next_index_array, next_offset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto take_last_until = [] <auto Data> {

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_offset{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data | std::views::reverse)
			{
				if constexpr (predicate_solver<Predicate>::template solve<Index>())
				{
					break;
				}
				--next_offset;
				next_index_array[next_offset] = Index;
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(... || [&] <auto Index> {
					if constexpr (predicate_solver<Predicate>::template solve<Index>())
					{
						return true;
					}
					--next_offset;
					next_index_array[next_offset] = Index;
					return false;
				}.template operator() < Data[Data.size - I - 1] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			std::size_t next_size{ Data.size - next_offset };

			return ViewData{ next_index_array, next_offset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto drop_last_while = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			constexpr std::size_t NextSize = []{

				std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data | std::views::reverse)
				{
					if constexpr (!predicate_solver<Predicate>::template solve<Index>())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (!predicate_solver<Predicate>::template solve<Index>())
						{
							return true;
						}
						--next_size;
						return false;
					}.template operator() < Data[Data.size - I - 1] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return next_size;
			}();

			std::array<index_type, NextSize> next_index_array{};

			for (std::size_t i{ 0 }; i < NextSize; ++i)
			{
				next_index_array[i] = Data[i];
			}

			return ViewData{ next_index_array, NextOffset, NextSize };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto drop_last_until = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			constexpr std::size_t NextSize = []{

				std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data | std::views::reverse)
				{
					if constexpr (predicate_solver<Predicate>::template solve<Index>())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (predicate_solver<Predicate>::template solve<Index>())
						{
							return true;
						}
						--next_size;
						return false;
					}.template operator() < Data[Data.size - I - 1] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return next_size;
			}();

			std::array<index_type, NextSize> next_index_array{};

			for (std::size_t i{ 0 }; i < NextSize; ++i)
			{
				next_index_array[i] = Data[i];
			}

			return ViewData{ next_index_array, NextOffset, NextSize };
		};

		//================================================================================

		template<typename Predicate>
		static constexpr auto select = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (predicate_solver<Predicate>::template solve<Index>())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (predicate_solver<Predicate>::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto remove = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (!predicate_solver<Predicate>::template solve<Index>())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (!predicate_solver<Predicate>::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto select_with_index = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (predicate_with_index_solver<Predicate>::template solve<Index>())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (predicate_with_index_solver<Predicate>::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto remove_with_index = [] <auto Data> {

			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Data.size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (!predicate_with_index_solver<Predicate>::template solve<Index>())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (!predicate_with_index_solver<Predicate>::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Data.size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//================================================================================

		template<typename EquivalenceRelation>
		static constexpr auto unique_first = [] <auto Data> {

			constexpr std::size_t Size{ Data.size };
			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr std::size_t I : std::views::iota(0uz, Size))
			{
				constexpr auto OuterIndex = Data[I];

				bool should_collect{ true };

				template for (constexpr std::size_t J : std::views::iota(0uz, I))
				{
					constexpr auto InnerIndex = Data[J];

					if constexpr (homogeneous_relation_solver<EquivalenceRelation>
						::template solve<OuterIndex, InnerIndex>())
					{
						should_collect = false;
						break;
					}
				}

				if (should_collect)
				{
					next_index_array[next_size] = OuterIndex;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto OuterIndex> {
					bool should_collect{ true };

					[&] <std::size_t... J> (std::index_sequence<J...>) {
						(... || [&] <auto InnerIndex> {
							if constexpr (homogeneous_relation_solver<EquivalenceRelation>
								::template solve<OuterIndex, InnerIndex>())
							{
								should_collect = false;
								return true;
							}
							return false;
						}.template operator() < Data[J] > ());
					}(std::make_index_sequence<I>{});

					if (should_collect)
					{
						next_index_array[next_size] = OuterIndex;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
		static constexpr auto unique_last = [] <auto Data> {

			constexpr std::size_t Size{ Data.size };
			constexpr std::size_t NextOffset{ 0 };

			std::array<index_type, Size> next_index_array{};
			std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr std::size_t I : std::views::iota(0uz, Size))
			{
				constexpr auto OuterIndex = Data[I];

				bool should_collect{ true };

				template for (constexpr std::size_t J : std::views::iota(I + 1, Size) | std::views::reverse)
				{
					constexpr auto InnerIndex = Data[J];

					if constexpr (homogeneous_relation_solver<EquivalenceRelation>
						::template solve<OuterIndex, InnerIndex>())
					{
						should_collect = false;
						break;
					}
				}

				if (should_collect)
				{
					next_index_array[next_size] = OuterIndex;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto OuterIndex> {
					bool should_collect{ true };

					[&] <std::size_t... J> (std::index_sequence<J...>) {
						(... || [&] <auto InnerIndex> {
							if constexpr (homogeneous_relation_solver<EquivalenceRelation>
								::template solve<OuterIndex, InnerIndex>())
							{
								should_collect = false;
								return true;
							}
							return false;
						}.template operator() < Data[Size - J - 1] > ());
					}(std::make_index_sequence<Size - I - 1>{});

					if (should_collect)
					{
						next_index_array[next_size] = OuterIndex;
						++next_size;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Size>{});
#endif

			return ViewData{ next_index_array, NextOffset, next_size };
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_first = [] <auto Data> {

			constexpr std::size_t Size{ Data.size };

			if constexpr (Size == 0) { return ViewData<index_type, 0>{ {}, 0, 0 }; }
			else
			{
				constexpr std::size_t NextOffset{ 0 };

				std::array<index_type, Size> next_index_array{ Data[0] };
				std::size_t next_size{ 1 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size - 1))
				{
					constexpr auto PreviousIndex{ Data[I] };
					constexpr auto CurrentIndex{ Data[I + 1] };

					if constexpr (!homogeneous_relation_solver<EquivalenceRelation>
						::template solve<PreviousIndex, CurrentIndex>())
					{
						next_index_array[next_size] = CurrentIndex;
						++next_size;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <std::size_t J> {
						constexpr auto PreviousIndex{ Data[J] };
						constexpr auto CurrentIndex{ Data[J + 1] };

						if constexpr (!homogeneous_relation_solver<EquivalenceRelation>
							::template solve<PreviousIndex, CurrentIndex>())
						{
							next_index_array[next_size] = CurrentIndex;
							++next_size;
						}
					}.template operator() < I > ());
				}(std::make_index_sequence<Size - 1>{});
#endif

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_last = [] <auto Data> {

			constexpr std::size_t Size{ Data.size };

			if constexpr (Size == 0) { return ViewData<index_type, 0>{ {}, 0, 0 }; }
			else
			{
				constexpr std::size_t NextOffset{ 0 };

				std::array<index_type, Size> next_index_array{};
				std::size_t next_size{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size - 1))
				{
					constexpr auto CurrentIndex{ Data[I] };
					constexpr auto NextIndex{ Data[I + 1] };

					if constexpr (!homogeneous_relation_solver<EquivalenceRelation>
						::template solve<CurrentIndex, NextIndex>())
					{
						next_index_array[next_size] = CurrentIndex;
						++next_size;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <std::size_t J> {
						constexpr auto CurrentIndex{ Data[J] };
						constexpr auto NextIndex{ Data[J + 1] };

						if constexpr (!homogeneous_relation_solver<EquivalenceRelation>
							::template solve<CurrentIndex, NextIndex>())
						{
							next_index_array[next_size] = CurrentIndex;
							++next_size;
						}
					}.template operator() < I > ());
				}(std::make_index_sequence<Size - 1>{});
#endif

				next_index_array[next_size] = Data[Size - 1];
				++next_size;

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//================================================================================

		template<auto Data, typename StrictWeakOrder>
			requires (Data.size == 0)
		static constexpr auto merge_sort()
		{ return ViewData<index_type, 0>{ {}, 0, 0 }; }

		template<auto Data, typename StrictWeakOrder>
			requires (Data.size == 1)
		static constexpr auto merge_sort()
		{ return ViewData<index_type, 1>{ { Data[0] }, 0, 1 }; }

		template<auto Data, typename StrictWeakOrder>
			requires (Data.size == 2)
		static constexpr auto merge_sort()
		{
			if constexpr (homogeneous_relation_solver<StrictWeakOrder>
				::template solve<Data[1], Data[0]>())
			{
				return ViewData<index_type, 2>{ { Data[1], Data[0] }, 0, 2 };
			}
			else
			{
				return ViewData<index_type, 2>{ { Data[0], Data[1] }, 0, 2 };
			}
		}

		template<auto LeftData, auto RightData, typename StrictWeakOrder>
		struct data_merger
		{
			static constexpr std::size_t LeftSize{ LeftData.size };
			static constexpr std::size_t RightSize{ RightData.size };
			static constexpr std::size_t Size{ LeftSize + RightSize };

			template<std::size_t LeftCursor, std::size_t RightCursor>
			static constexpr void merge(std::array<index_type, Size>& merged_index_array)
			{
				constexpr std::size_t MergedSize{ LeftCursor + RightCursor };

				if constexpr (LeftCursor == LeftSize)
				{
					for (std::size_t i{ MergedSize }, j{ RightCursor }; j < RightSize; ++i, ++j)
					{
						merged_index_array[i] = RightData[j];
					}
				}
				else if constexpr (RightCursor == RightSize)
				{
					for (std::size_t i{ MergedSize }, j{ LeftCursor }; j < LeftSize; ++i, ++j)
					{
						merged_index_array[i] = LeftData[j];
					}
				}
				else
				{
					if constexpr (homogeneous_relation_solver<StrictWeakOrder>
						::template solve<RightData[RightCursor], LeftData[LeftCursor]>())
					{
						merged_index_array[MergedSize] = RightData[RightCursor];
						merge<LeftCursor, RightCursor + 1>(merged_index_array);
					}
					else
					{
						merged_index_array[MergedSize] = LeftData[LeftCursor];
						merge<LeftCursor + 1, RightCursor>(merged_index_array);
					}
				}
			}
		};

		template<auto Data, typename StrictWeakOrder>
		static constexpr auto merge_sort()
		{
			constexpr std::size_t Size{ Data.size };
			constexpr std::size_t LeftSize{ Size / 2 };
			constexpr std::size_t RightSize{ Size - LeftSize };

			constexpr auto LeftData = []{

				std::array<index_type, LeftSize> left_index_array{};

				for (std::size_t i{ 0 }; i < LeftSize; ++i)
				{
					left_index_array[i] = Data[i];
				}

				return ViewData{ left_index_array, 0, LeftSize };
			}();

			constexpr auto RightData = []{

				std::array<index_type, RightSize> right_index_array{};

				for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
				{
					right_index_array[i] = Data[j];
				}

				return ViewData{ right_index_array, 0, RightSize };
			}();

			constexpr auto SortedLeftData{ merge_sort<LeftData, StrictWeakOrder>() };
			constexpr auto SortedRightData{ merge_sort<RightData, StrictWeakOrder>() };

			std::array<index_type, Size> merged_index_array{};

			data_merger<SortedLeftData, SortedRightData, StrictWeakOrder>
				::template merge<0, 0>(merged_index_array);

			return ViewData{ merged_index_array, 0, Size };
		}

		template<typename StrictWeakOrder>
		static constexpr auto sort = [] <auto Data> { return merge_sort<Data, StrictWeakOrder>(); };

		//================================================================================

		template<auto Data, typename PartialOrder>
			requires (Data.size == 0)
		static constexpr auto merge_sort_with_partial_order()
		{ return ViewData<index_type, 0>{ {}, 0, 0 }; }

		template<auto Data, typename PartialOrder>
			requires (Data.size == 1)
		static constexpr auto merge_sort_with_partial_order()
		{ return ViewData<index_type, 1>{ { Data[0] }, 0, 1 }; }

		template<auto Data, typename PartialOrder>
			requires (Data.size == 2)
		static constexpr auto merge_sort_with_partial_order()
		{
			if constexpr (homogeneous_relation_solver<PartialOrder>
				::template solve<Data[0], Data[1]>())
			{
				return ViewData<index_type, 2>{ { Data[0], Data[1] }, 0, 2 };
			}
			else
			{
				return ViewData<index_type, 2>{ { Data[1], Data[0] }, 0, 2 };
			}
		}

		template<auto LeftData, auto RightData, typename PartialOrder>
		struct data_merger_with_partial_order
		{
			static constexpr std::size_t LeftSize{ LeftData.size };
			static constexpr std::size_t RightSize{ RightData.size };
			static constexpr std::size_t Size{ LeftSize + RightSize };

			template<std::size_t LeftCursor, std::size_t RightCursor>
			static constexpr void merge(std::array<index_type, Size>& merged_index_array)
			{
				constexpr std::size_t MergedSize{ LeftCursor + RightCursor };

				if constexpr (LeftCursor == LeftSize)
				{
					for (std::size_t i{ MergedSize }, j{ RightCursor }; j < RightSize; ++i, ++j)
					{
						merged_index_array[i] = RightData[j];
					}
				}
				else if constexpr (RightCursor == RightSize)
				{
					for (std::size_t i{ MergedSize }, j{ LeftCursor }; j < LeftSize; ++i, ++j)
					{
						merged_index_array[i] = LeftData[j];
					}
				}
				else
				{
					if constexpr (homogeneous_relation_solver<PartialOrder>
						::template solve<LeftData[LeftCursor], RightData[RightCursor]>())
					{
						merged_index_array[MergedSize] = LeftData[LeftCursor];
						merge<LeftCursor + 1, RightCursor>(merged_index_array);
					}
					else
					{
						merged_index_array[MergedSize] = RightData[RightCursor];
						merge<LeftCursor, RightCursor + 1>(merged_index_array);
					}
				}
			}
		};

		template<auto Data, typename PartialOrder>
		static constexpr auto merge_sort_with_partial_order()
		{
			constexpr std::size_t Size{ Data.size };
			constexpr std::size_t LeftSize{ Size / 2 };
			constexpr std::size_t RightSize{ Size - LeftSize };

			constexpr auto LeftData = []{

				std::array<index_type, LeftSize> left_index_array{};

				for (std::size_t i{ 0 }; i < LeftSize; ++i)
				{
					left_index_array[i] = Data[i];
				}

				return ViewData{ left_index_array, 0, LeftSize };
			}();

			constexpr auto RightData = []{

				std::array<index_type, RightSize> right_index_array{};

				for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
				{
					right_index_array[i] = Data[j];
				}

				return ViewData{ right_index_array, 0, RightSize };
			}();

			constexpr auto SortedLeftData{ merge_sort_with_partial_order<LeftData, PartialOrder>() };
			constexpr auto SortedRightData{ merge_sort_with_partial_order<RightData, PartialOrder>() };

			std::array<index_type, Size> merged_index_array{};

			data_merger_with_partial_order<SortedLeftData, SortedRightData, PartialOrder>
				::template merge<0, 0>(merged_index_array);

			return ViewData{ merged_index_array, 0, Size };
		}

		template<auto Data, typename PartialOrder>
		static constexpr auto sort_with_partial_order_impl()
		{
			constexpr std::size_t Size{ Data.size };

			constexpr auto ReflexiveData = []{

				std::array<index_type, Size> reflexive_index_array{};
				std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (homogeneous_relation_solver<PartialOrder>
						::template solve<Index, Index>())
					{
						reflexive_index_array[reflexive_size] = Index;
						++reflexive_size;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (homogeneous_relation_solver<PartialOrder>
							::template solve<Index, Index>())
						{
							reflexive_index_array[reflexive_size] = Index;
							++reflexive_size;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				return ViewData{ reflexive_index_array, 0, reflexive_size };
			}();

			constexpr std::size_t ReflexiveSize{ ReflexiveData.size };

			if constexpr (ReflexiveSize == 0) { return Data; }

			std::array<index_type, Size> sorted_index_array{};
			std::size_t irreflexive_cursor{ ReflexiveSize };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
			//C++26
			template for (constexpr auto Index : Data)
			{
				if constexpr (!homogeneous_relation_solver<PartialOrder>
					::template solve<Index, Index>())
				{
					sorted_index_array[irreflexive_cursor] = Index;
					++irreflexive_cursor;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (!homogeneous_relation_solver<PartialOrder>
						::template solve<Index, Index>())
					{
						sorted_index_array[irreflexive_cursor] = Index;
						++irreflexive_cursor;
					}
				}.template operator() < Data[I] > ());
			}(std::make_index_sequence<Size>{});
#endif

			constexpr auto SortedReflexiveData{ merge_sort_with_partial_order<ReflexiveData, PartialOrder>() };

			for (std::size_t i{ 0 }; i < ReflexiveSize; ++i)
			{
				sorted_index_array[i] = SortedReflexiveData[i];
			}

			return ViewData{ sorted_index_array, 0, Size };
		}

		template<typename PartialOrder>
		static constexpr auto sort_with_partial_order = [] <auto Data> {
			return sort_with_partial_order_impl<Data, PartialOrder>();
		};

		//--------------------------------------------------------------------------------

		template<typename PartialOrder>
		static constexpr auto unique_first_with_partial_order = [] <auto Data> {



		};

		//--------------------------------------------------------------------------------

		template<typename PartialOrder>
		static constexpr auto unique_last_with_partial_order = [] <auto Data> {



		};

	};


	template<typename List>
	struct type_list_bool_solver
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		template<typename Predicate>
		struct predicate_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					typename List::template element<Index>
				> () };
			};

			template<auto Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					typename List::template element<Index>
				> ();
			}
		};

		template<typename Predicate>
		struct predicate_with_index_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					typename List::template element<Index>
				> () };
			};

			template<auto Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					typename List::template element<Index>
				> ();
			}
		};

		template<typename HomogeneousRelation>
		struct homogeneous_relation_solver
		{
			template<auto Index1, auto Index2>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					typename List::template element<Index1>,
					typename List::template element<Index2>
				> () };
			};

			template<auto Index1, auto Index2>
			static constexpr bool solve()
			{
				return std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					typename List::template element<Index1>,
					typename List::template element<Index2>
				> ();
			}
		};

	};

	template<typename List>
	struct value_list_bool_solver
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		template<typename Predicate>
		struct predicate_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					List::template element<Index>
				> () };
			};

			template<auto Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					List::template element<Index>
				> ();
			}
		};

		template<typename Predicate>
		struct predicate_with_index_solver
		{
			template<auto Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					List::template element<Index>
				> () };
			};

			template<auto Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					List::template element<Index>
				> ();
			}
		};

		template<typename HomogeneousRelation>
		struct homogeneous_relation_solver
		{
			template<auto Index1, auto Index2>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					List::template element<Index1>,
					List::template element<Index2>
				> () };
			};

			template<auto Index1, auto Index2>
			static constexpr bool solve()
			{
				return std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					List::template element<Index1>,
					List::template element<Index2>
				> ();
			}
		};

	};
	
	struct type_list_error_reporter
	{
		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");
		}

		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate_with_index()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Index, typename Type>() -> bool { ... }");
		}

		template<typename EquivalenceRelation>
		static constexpr void incompatible_non_capturing_equivalence_relation()
		{
			static_assert(dependent_false_v<EquivalenceRelation>,
				"[logicwise] Error: Incompatible equivalence relation signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

		template<typename StrictWeakOrder>
		static constexpr void incompatible_non_capturing_strict_weak_order()
		{
			static_assert(dependent_false_v<StrictWeakOrder>,
				"[logicwise] Error: Incompatible strict weak order signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

		template<typename PartialOrder>
		static constexpr void incompatible_non_capturing_partial_order()
		{
			static_assert(dependent_false_v<PartialOrder>,
				"[logicwise] Error: Incompatible partial order signature!\n"
				"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");
		}

	};

	struct value_list_error_reporter
	{
		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Value>() -> bool { ... }");
		}

		template<typename Predicate>
		static constexpr void incompatible_non_capturing_predicate_with_index()
		{
			static_assert(dependent_false_v<Predicate>,
				"[logicwise] Error: Incompatible predicate signature!\n"
				"Expected non-capturing lambda: [] <auto Index, auto Value>() -> bool { ... }");
		}

		template<typename EquivalenceRelation>
		static constexpr void incompatible_non_capturing_equivalence_relation()
		{
			static_assert(dependent_false_v<EquivalenceRelation>,
				"[logicwise] Error: Incompatible equivalence relation signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename StrictWeakOrder>
		static constexpr void incompatible_non_capturing_strict_weak_order()
		{
			static_assert(dependent_false_v<StrictWeakOrder>,
				"[logicwise] Error: Incompatible strict weak order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

		template<typename PartialOrder>
		static constexpr void incompatible_non_capturing_partial_order()
		{
			static_assert(dependent_false_v<PartialOrder>,
				"[logicwise] Error: Incompatible partial order signature!\n"
				"Expected non-capturing lambda: [] <auto Value1, auto Value2>() -> bool { ... }");
		}

	};

	template<typename List>
	struct type_list_view_protocol
	{
		using bool_solver = type_list_bool_solver<List>;
		using dependent_view_adaptor = view_adaptor<bool_solver>;
		using error_reporter = type_list_error_reporter;

		template<auto Data>
		struct materializer
		{
			template<template<typename...> typename TypeWrapper = type_list>
			static constexpr auto bake()
			{
				return[] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename List::template element<Data[I]>...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				typename decltype(std::remove_reference_t<Mapping>{}.template operator()
					< typename List::template element<Data[0]> > ())::type;
			}
			static constexpr auto forge_into_type(Mapping&&)
			{
				return[] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename decltype(std::remove_reference_t<Mapping>{}.template operator()
							< typename List::template element<Data[I]> > ())::type...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<typename> typename Mapping, template<typename...> typename TypeWrapper = type_list>
			static constexpr auto forge_into_type()
			{
				return[] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper <
						Mapping< typename List::template element<Data[I]> >...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				std::remove_reference_t<Mapping>{}.template operator()
					< typename List::template element<Data[0]> > ();
			}
			static constexpr auto forge_into_value(Mapping&&)
			{
				return[] <std::size_t... I> (std::index_sequence<I...>) {
					return ValueWrapper <
						std::remove_reference_t<Mapping>{}.template operator()
						< typename List::template element<Data[I]> > ()...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<typename MappingType>
				requires (Data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< typename List::template element<Data[0]> > ();
			}
			static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				if constexpr (Data.size == 0)
				{
					return std::array<std::monostate, 0>{};
				}
				else
				{
					return[&] <std::size_t... I> (std::index_sequence<I...>) {
						return std::array{
							mapping.template operator()
								< typename List::template element<Data[I]> > ()...
						};
					}(std::make_index_sequence<Data.size>{});
				}
			}

			//--------------------------------------------------------------------------------

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
			static constexpr auto forge_into_type(Mapping&&)
			{
				static_assert(dependent_false_v<Mapping>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> std::type_identity<...> { ... }");

				return TypeWrapper<>{};
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
			static constexpr auto forge_into_value(Mapping&&)
			{
				static_assert(dependent_false_v<Mapping>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> auto { ... }");

				return ValueWrapper<>{};
			}

			template<typename MappingType>
			static constexpr auto forge_into_instance(MappingType&&)
			{
				static_assert(dependent_false_v<MappingType>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected: [] <typename Type>() -> auto { ... }");

				return std::array<std::monostate, 0>{};
			}

		};

	};

	template<typename List>
	struct value_list_view_protocol
	{
		using bool_solver = value_list_bool_solver<List>;
		using dependent_view_adaptor = view_adaptor<bool_solver>;
		using error_reporter = value_list_error_reporter;

		template<auto Data>
		struct materializer
		{
			template<template<auto...> typename ValueWrapper = value_list>
			static constexpr auto bake()
			{
				return[] <std::size_t... I> (std::index_sequence<I...>)
				{
					return ValueWrapper<
						List::template element<Data[I]>...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				typename decltype(std::remove_reference_t<Mapping>{}.template operator()
					< List::template element<Data[0]> > ())::type;
			}
			static constexpr auto forge_into_type(Mapping&&)
			{
				return[] <std::size_t... I> (std::index_sequence<I...>)
				{
					return TypeWrapper<
						typename decltype(std::remove_reference_t<Mapping>{}.template operator()
							< List::template element<Data[I]> > ())::type...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<auto> typename Mapping, template<typename...> typename TypeWrapper = type_list>
			static constexpr auto forge_into_type()
			{
				return[] <std::size_t... I> (std::index_sequence<I...>)
				{
					return TypeWrapper <
						Mapping< List::template element<Data[I]> >...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				std::remove_reference_t<Mapping>{}.template operator()
					< List::template element<Data[0]> > ();
			}
			static constexpr auto forge_into_value(Mapping&&)
			{
				return[] <std::size_t... I> (std::index_sequence<I...>)
				{
					return ValueWrapper <
						std::remove_reference_t<Mapping>{}.template operator()
						< List::template element<Data[I]> > ()...
					> {};
				}(std::make_index_sequence<Data.size>{});
			}

			template<typename MappingType>
				requires (Data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< List::template element<Data[0]> > ();
			}
			static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				if constexpr (Data.size == 0)
				{
					return std::array<std::monostate, 0>{};
				}
				else
				{
					return[&] <std::size_t... I> (std::index_sequence<I...>)
					{
						return std::array{
							mapping.template operator()
								< List::template element<Data[I]> > ()...
						};
					}(std::make_index_sequence<Data.size>{});
				}
			}

			//--------------------------------------------------------------------------------

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
			static constexpr auto forge_into_type(Mapping&&)
			{
				static_assert(dependent_false_v<Mapping>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected non-capturing lambda: [] <auto Value>() -> std::type_identity<...> { ... }");

				return TypeWrapper<>{};
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
			static constexpr auto forge_into_value(Mapping&&)
			{
				static_assert(dependent_false_v<Mapping>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected non-capturing lambda: [] <auto Value>() -> auto { ... }");

				return ValueWrapper<>{};
			}

			template<typename MappingType>
			static constexpr auto forge_into_instance(MappingType&&)
			{
				static_assert(dependent_false_v<MappingType>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected: [] <auto Value>() -> auto { ... }");

				return std::array<std::monostate, 0>{};
			}

		};

	};


	template<typename ViewProtocol, auto Data>
	struct concrete_view : ViewProtocol::template materializer<Data>
	{
		using bool_solver				= typename ViewProtocol::bool_solver;
		using dependent_view_adaptor	= typename ViewProtocol::dependent_view_adaptor;
		using error_reporter			= typename ViewProtocol::error_reporter;

		using CurrentView = concrete_view;

		struct private_constraint
		{
			template<typename Predicate>
			static constexpr bool valid_predicate_for_first_element =
				(Data.size == 0) || requires
			{
				requires bool_solver::template predicate_solver<Predicate>::template solvable<Data[0]>;
			};

			template<typename Predicate>
			static constexpr bool valid_predicate_for_last_element =
				(Data.size == 0) || requires
			{
				requires bool_solver::template predicate_solver<Predicate>::template solvable<Data[Data.size - 1]>;
			};

			template<typename Predicate>
			static constexpr bool valid_predicate_with_index =
				(Data.size == 0) || requires
			{
				requires bool_solver::template predicate_with_index_solver<Predicate>::template solvable<Data[0]>;
			};

			template<typename HomogeneousRelation>
			static constexpr bool valid_homogeneous_relation =
				(Data.size == 0) || requires
			{
				requires bool_solver::template homogeneous_relation_solver<HomogeneousRelation>::template solvable<Data[0], Data[0]>;
			};

		};

		//--------------------------------------------------------------------------------

		static constexpr auto reverse()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::reverse.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		static constexpr auto take()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::take<Count>.template operator() < Data > () > {};
		}

		template<std::size_t Count>
		static constexpr auto drop()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::drop<Count>.template operator() < Data > () > {};
		}

		template<std::size_t Count>
		static constexpr auto take_last()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::take_last<Count>.template operator() < Data > () > {};
		}

		template<std::size_t Count>
		static constexpr auto drop_last()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::drop_last<Count>.template operator() < Data > () > {};
		}

		template<std::size_t Start, std::size_t End>
		static constexpr auto slice()
		{
			return concrete_view < ViewProtocol, non_dependent_view_adaptor
				::slice<Start, End>.template operator() < Data > () >{};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto take_while(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template take_while<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto take_until(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template take_until<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto drop_while(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template drop_while<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto drop_until(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template drop_until<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
		static constexpr auto take_last_while(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template take_last_while<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
		static constexpr auto take_last_until(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template take_last_until<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
		static constexpr auto drop_last_while(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template drop_last_while<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
		static constexpr auto drop_last_until(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template drop_last_until<Predicate>.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto select(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template select<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
		static constexpr auto remove(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template remove<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_with_index<Predicate>)
		static constexpr auto select_with_index(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template select_with_index<Predicate>.template operator() < Data > () > {};
		}

		template<typename Predicate>
			requires (private_constraint::template valid_predicate_with_index<Predicate>)
		static constexpr auto remove_with_index(Predicate&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template remove_with_index<Predicate>.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
			requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
		static constexpr auto unique_first(EquivalenceRelation&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_first<EquivalenceRelation>.template operator() < Data > () > {};
		}

		template<typename EquivalenceRelation>
			requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
		static constexpr auto unique_last(EquivalenceRelation&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_last<EquivalenceRelation>.template operator() < Data > () > {};
		}

		template<typename EquivalenceRelation>
			requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
		static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_adjacent_first<EquivalenceRelation>.template operator() < Data > () > {};
		}

		template<typename EquivalenceRelation>
			requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
		static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_adjacent_last<EquivalenceRelation>.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		//严格弱序关系(Strict Weak Order)需要满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)
		//以及不可比关系的传递性(Transitivity of Incomparability)。
		//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
		template<typename StrictWeakOrder>
			requires (private_constraint::template valid_homogeneous_relation<StrictWeakOrder>)
		static constexpr auto sort(StrictWeakOrder&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template sort<StrictWeakOrder>.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		//偏序关系(Partial Order)原则上要求所有元素满足自反性(Reflexivity)，但是 sort_with_partial_order 会把反自反的元素按原顺序放在最后。
		//除此之外偏序关系(Partial Order)还要满足反对称性(Antisymmetry)、传递性(Transitivity)。
		template<typename PartialOrder>
			requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
		static constexpr auto sort_with_partial_order(PartialOrder&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template sort_with_partial_order<PartialOrder>.template operator() < Data > () > {};
		}

		template<typename PartialOrder>
			requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
		static constexpr auto unique_first_with_partial_order(PartialOrder&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_first_with_partial_order<PartialOrder>.template operator() < Data > () > {};
		}

		template<typename PartialOrder>
			requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
		static constexpr auto unique_last_with_partial_order(PartialOrder&&)
		{
			return concrete_view < ViewProtocol, dependent_view_adaptor
				::template unique_last_with_partial_order<PartialOrder>.template operator() < Data > () > {};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto take_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_last_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_last_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_last_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_last_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto select(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto remove(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto select_with_index(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate_with_index<Predicate>();

			return CurrentView{};
		}
		
		template<typename Predicate>
		static constexpr auto remove_with_index(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate_with_index<Predicate>();

			return CurrentView{};
		}
		
		template<typename EquivalenceRelation>
		static constexpr auto unique_first(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_last(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename StrictWeakOrder>
		static constexpr auto sort(StrictWeakOrder&&)
		{
			error_reporter::template incompatible_non_capturing_strict_weak_order<StrictWeakOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto sort_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto unique_first_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto unique_last_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

	};


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

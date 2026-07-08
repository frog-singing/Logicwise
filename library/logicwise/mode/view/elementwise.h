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

	struct elementwise_view;

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
		using index_type = typename BoolSolver::index_type;

		template<typename Predicate>
		static constexpr auto predicate_solver{ BoolSolver::template predicate_solver<Predicate> };

		template<typename Predicate>
		static constexpr auto predicate_with_index_solver{ BoolSolver::template predicate_with_index_solver<Predicate> };

		template<typename HomogeneousRelation>
		static constexpr auto homogeneous_relation_solver{ BoolSolver::template homogeneous_relation_solver<HomogeneousRelation> };

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
				if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
					if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
					{
						break;
					}
					--next_size;
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(... || [&] <auto Index> {
						if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (!predicate_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (predicate_with_index_solver<Predicate>.template operator() < Index > ())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (predicate_with_index_solver<Predicate>.template operator() < Index > ())
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
				if constexpr (!predicate_with_index_solver<Predicate>.template operator() < Index > ())
				{
					next_index_array[next_size] = Index;
					++next_size;
				}
			}
#else
			//C++20
			[&] <std::size_t... I> (std::index_sequence<I...>) {
				(..., [&] <auto Index> {
					if constexpr (!predicate_with_index_solver<Predicate>.template operator() < Index > ())
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
						.template operator() < OuterIndex, InnerIndex > ())
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
								.template operator() < OuterIndex, InnerIndex > ())
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
						.template operator() < OuterIndex, InnerIndex > ())
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
								.template operator() < OuterIndex, InnerIndex > ())
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
						.template operator() < PreviousIndex, CurrentIndex > ())
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
							.template operator() < PreviousIndex, CurrentIndex > ())
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
						.template operator() < CurrentIndex, NextIndex > ())
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
							.template operator() < CurrentIndex, NextIndex > ())
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
				.template operator() < Data[1], Data[0] > ())
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
						.template operator() < RightData[RightCursor], LeftData[LeftCursor] > ())
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
				.template operator() < Data[0], Data[1] > ())
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
						.template operator() < LeftData[LeftCursor], RightData[RightCursor] > ())
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
						.template operator() < Index, Index > ())
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
							.template operator() < Index, Index > ())
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
					.template operator() < Index, Index > ())
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
						.template operator() < Index, Index > ())
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
		static constexpr auto predicate_solver = [] <auto Index> {

			return Predicate{}.template operator() < typename List::template element<Index> > ();
		};

		template<typename Predicate>
		static constexpr auto predicate_with_index_solver = [] <auto Index> {

			return Predicate{}.template operator() < Index, typename List::template element<Index> > ();
		};

		template<typename HomogeneousRelation>
		static constexpr auto homogeneous_relation_solver = [] <auto Index1, auto Index2> {

			return HomogeneousRelation{}.template operator() <
				typename List::template element<Index1>,
				typename List::template element<Index2>
			> ();
		};

	};

	template<typename List>
	struct value_list_bool_solver
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		template<typename Predicate>
		static constexpr auto predicate_solver = [] <auto Index> {

			return Predicate{}.template operator() < List::template element<Index> > ();
		};

		template<typename Predicate>
		static constexpr auto predicate_with_index_solver = [] <auto Index> {

			return Predicate{}.template operator() < Index, List::template element<Index> > ();
		};

		template<typename HomogeneousRelation>
		static constexpr auto homogeneous_relation_solver = [] <auto Index1, auto Index2> {

			return HomogeneousRelation{}.template operator() <
				List::template element<Index1>,
				List::template element<Index2>
			> ();
		};

	};
	

	struct elementwise_view
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		template<typename PreviousView, auto ViewAdaptor = non_dependent_view_adaptor::identity>
		struct type_list_view
		{
			using list = typename PreviousView::list;
			
			struct private_data
			{
				static constexpr auto data{ ViewAdaptor.template operator() < PreviousView::private_data::get_data() > () };
				static constexpr auto get_data() noexcept { return data; }
			};


			using CurrentView = type_list_view;


			using BoolSolver = type_list_bool_solver<list>;
			using DependentViewAdaptor = view_adaptor<BoolSolver>;

			struct private_constraint
			{
				template<typename Predicate>
				static constexpr bool valid_predicate_for_first_element =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_solver<Predicate>
						.template operator() < private_data::data[0] > () };
				};

				template<typename Predicate>
				static constexpr bool valid_predicate_for_last_element =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_solver<Predicate>
						.template operator() < private_data::data[private_data::data.size - 1] > () };
				};

				template<typename Predicate>
				static constexpr bool valid_predicate_with_index =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_with_index_solver<Predicate>
						.template operator() < private_data::data[0] > () };
				};

				template<typename HomogeneousRelation>
				static constexpr bool valid_homogeneous_relation =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template homogeneous_relation_solver<HomogeneousRelation>
						.template operator() < private_data::data[0], private_data::data[0] > () };
				};

			};


			static constexpr auto reverse() { return type_list_view < CurrentView, non_dependent_view_adaptor::reverse > {}; }

			//--------------------------------------------------------------------------------

			template<std::size_t Count>
			static constexpr auto take() { return type_list_view < CurrentView, non_dependent_view_adaptor::take<Count> > {}; }

			template<std::size_t Count>
			static constexpr auto drop() { return type_list_view < CurrentView, non_dependent_view_adaptor::drop<Count> > {}; }

			template<std::size_t Count>
			static constexpr auto take_last() { return type_list_view < CurrentView, non_dependent_view_adaptor::take_last<Count> > {}; }

			template<std::size_t Count>
			static constexpr auto drop_last() { return type_list_view < CurrentView, non_dependent_view_adaptor::drop_last<Count> > {}; }

			template<std::size_t Start, std::size_t End>
			static constexpr auto slice() { return type_list_view < CurrentView, non_dependent_view_adaptor::slice<Start, End> >{}; }

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto take_while(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template take_while<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto take_until(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template take_until<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto drop_while(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template drop_while<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto drop_until(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template drop_until<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
			static constexpr auto take_last_while(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template take_last_while<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
			static constexpr auto take_last_until(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template take_last_until<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
			static constexpr auto drop_last_while(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template drop_last_while<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_last_element<Predicate>)
			static constexpr auto drop_last_until(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template drop_last_until<Predicate> > {};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto select(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template select<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_for_first_element<Predicate>)
			static constexpr auto remove(Predicate&&)
			{ return type_list_view < CurrentView, DependentViewAdaptor::template remove<Predicate> > {}; }

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_with_index<Predicate>)
			static constexpr auto select_with_index(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template select_with_index<Predicate> > {};
			}

			template<typename Predicate>
				requires (private_constraint::template valid_predicate_with_index<Predicate>)
			static constexpr auto remove_with_index(Predicate&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template remove_with_index<Predicate> > {};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
				requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
			static constexpr auto unique_first(EquivalenceRelation&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_first<EquivalenceRelation> > {};
			}

			template<typename EquivalenceRelation>
				requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
			static constexpr auto unique_last(EquivalenceRelation&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_last<EquivalenceRelation> > {};
			}

			template<typename EquivalenceRelation>
				requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
			static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_adjacent_first<EquivalenceRelation> > {};
			}

			template<typename EquivalenceRelation>
				requires (private_constraint::template valid_homogeneous_relation<EquivalenceRelation>)
			static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_adjacent_last<EquivalenceRelation> > {};
			}

			//--------------------------------------------------------------------------------

			//严格弱序关系(Strict Weak Order)需要满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)
			//以及不可比关系的传递性(Transitivity of Incomparability)。
			//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
			template<typename StrictWeakOrder>
				requires (private_constraint::template valid_homogeneous_relation<StrictWeakOrder>)
			static constexpr auto sort(StrictWeakOrder&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template sort<StrictWeakOrder> > {};
			}

			//--------------------------------------------------------------------------------

			//偏序关系(Partial Order)原则上要求所有元素满足自反性(Reflexivity)，但是 sort_with_partial_order 会把反自反的元素按原顺序放在最后。
			//除此之外偏序关系(Partial Order)还要满足反对称性(Antisymmetry)、传递性(Transitivity)。
			template<typename PartialOrder>
				requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
			static constexpr auto sort_with_partial_order(PartialOrder&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template sort_with_partial_order<PartialOrder> > {};
			}

			template<typename PartialOrder>
				requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
			static constexpr auto unique_first_with_partial_order(PartialOrder&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_first_with_partial_order<PartialOrder> > {};
			}

			template<typename PartialOrder>
				requires (private_constraint::template valid_homogeneous_relation<PartialOrder>)
			static constexpr auto unique_last_with_partial_order(PartialOrder&&)
			{
				return type_list_view < CurrentView, DependentViewAdaptor::template unique_last_with_partial_order<PartialOrder> > {};
			}

			//--------------------------------------------------------------------------------

			template<template<typename...> typename TypeWrapper = type_list>
				requires requires { private_data::data; } //用于创建非特化路径的 bake() 来提示 IDE 返回类型
			static constexpr auto bake()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename list::template element<private_data::data[I]>...
					> {};
				}(std::make_index_sequence<private_data::data.size>{});
			}

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
				requires (private_data::data.size == 0) || requires
			{
				typename decltype(Mapping{}.template operator()
					< typename list::template element<private_data::data[0]> > ())::type;
			}
			static constexpr auto forge_into_type(Mapping&&)
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename decltype(Mapping{}.template operator()
							< typename list::template element<private_data::data[I]> > ())::type...
					> {};
				}(std::make_index_sequence<private_data::data.size>{});
			}

			template<template<typename> typename Mapping, template<typename...> typename TypeWrapper = type_list>
			static constexpr auto forge_into_type()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper <
						Mapping< typename list::template element<private_data::data[I]> >...
					> {};
				}(std::make_index_sequence<private_data::data.size>{});
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
				requires (private_data::data.size == 0) || requires
			{
				Mapping{}.template operator()
					< typename list::template element<private_data::data[0]> > ();
			}
			static constexpr auto forge_into_value(Mapping&&)
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return ValueWrapper<
						Mapping{}.template operator()
							< typename list::template element<private_data::data[I]> > ()...
					> {};
				}(std::make_index_sequence<private_data::data.size>{});
			}

			template<typename MappingType>
				requires (private_data::data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< typename list::template element<private_data::data[0]> > ();
			}
			static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				if constexpr (private_data::data.size == 0)
				{
					return std::array<std::monostate, 0>{};
				}
				else
				{
					return[&] <std::size_t... I> (std::index_sequence<I...>) {
						return std::array{
							mapping.template operator()
								< typename list::template element<private_data::data[I]> > ()...
						};
					}(std::make_index_sequence<private_data::data.size>{});
				}
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
			static constexpr auto take_while(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_until(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_while(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_until(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_last_while(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_last_until(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_last_while(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_last_until(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto select(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto remove(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto select_with_index(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <auto Index, typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto remove_with_index(Predicate&&)
			{
				static_assert(dependent_false_v<Predicate>,
					"[logicwise] Error: Incompatible predicate signature!\n"
					"Expected non-capturing lambda: [] <auto Index, typename Type>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_first(EquivalenceRelation&&)
			{
				static_assert(dependent_false_v<EquivalenceRelation>,
					"[logicwise] Error: Incompatible equivalence relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_last(EquivalenceRelation&&)
			{
				static_assert(dependent_false_v<EquivalenceRelation>,
					"[logicwise] Error: Incompatible equivalence relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
			{
				static_assert(dependent_false_v<EquivalenceRelation>,
					"[logicwise] Error: Incompatible equivalence relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
			{
				static_assert(dependent_false_v<EquivalenceRelation>,
					"[logicwise] Error: Incompatible equivalence relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename StrictWeakOrder>
			static constexpr auto sort(StrictWeakOrder&&)
			{
				static_assert(dependent_false_v<StrictWeakOrder>,
					"[logicwise] Error: Incompatible strict weak order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename PartialOrder>
			static constexpr auto sort_with_partial_order(PartialOrder&&)
			{
				static_assert(dependent_false_v<PartialOrder>,
					"[logicwise] Error: Incompatible partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename PartialOrder>
			static constexpr auto unique_first_with_partial_order(PartialOrder&&)
			{
				static_assert(dependent_false_v<PartialOrder>,
					"[logicwise] Error: Incompatible partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename PartialOrder>
			static constexpr auto unique_last_with_partial_order(PartialOrder&&)
			{
				static_assert(dependent_false_v<PartialOrder>,
					"[logicwise] Error: Incompatible partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			//用于提示 IDE 返回类型
			template<template<typename...> typename TypeWrapper = type_list>
			static constexpr auto bake()
			{
#if defined(__cpp_lib_unreachable) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_23
				//C++23
				std::unreachable();
#else
				//C++20
				static_assert(dependent_false_v<TypeWrapper<>>, "[logicwise] Error: Unreachable.");
#endif
				return TypeWrapper<>{};
			}

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

		template<typename PreviousView, auto ViewAdaptor = non_dependent_view_adaptor::identity>
		struct value_list_view
		{
			using list = typename PreviousView::list;

			struct private_data
			{
				static constexpr auto data{ ViewAdaptor.template operator() < PreviousView::private_data::get_data() > () };
				static constexpr auto get_data() noexcept { return data; }
			};


			using CurrentView = value_list_view;


			using BoolSolver = value_list_bool_solver<list>;
			using DependentViewAdaptor = view_adaptor<BoolSolver>;

			struct private_constraint
			{
				template<typename Predicate>
				static constexpr bool valid_predicate_for_first_element =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_solver<Predicate>
						.template operator() < private_data::data[0] > () };
				};

				template<typename Predicate>
				static constexpr bool valid_predicate_for_last_element =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_solver<Predicate>
						.template operator() < private_data::data[private_data::data.size - 1] > () };
				};

				template<typename Predicate>
				static constexpr bool valid_predicate_with_index =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template predicate_with_index_solver<Predicate>
						.template operator() < private_data::data[0] > () };
				};

				template<typename HomogeneousRelation>
				static constexpr bool valid_homogeneous_relation =
					(private_data::data.size == 0) || requires
				{
					bool{ BoolSolver::template homogeneous_relation_solver<HomogeneousRelation>
						.template operator() < private_data::data[0], private_data::data[0] > () };
				};

			};




		};

	};


	template<typename Mode, typename Arrangement>
	class elementwise_viewing
	{
	public:
		template<typewise::List WrapperInstance>
		[[nodiscard]] static constexpr auto in()
		{
			return elementwise_view::type_list_view<initial_wrapper_instance_view<as_type_list<WrapperInstance>>>{};
		}

		template<valuewise::List WrapperInstance>
		[[nodiscard]] static constexpr auto in()
		{
			return elementwise_view::value_list_view<initial_wrapper_instance_view<as_value_list<WrapperInstance>>>{};
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

		template<typename WrapperInstance>
		struct initial_wrapper_instance_view
		{
			using list = WrapperInstance;

			static constexpr extent_type Extent{ list::size };
			using IndexSequencer = index_sequencer<Arrangement, IndexTraverserType, Extent>;
			static constexpr auto IndexArray{ IndexSequencer::generate_index_array() };

			struct private_data
			{
				static constexpr auto get_data() noexcept { return ViewData{ IndexArray, std::size_t{ 0 }, IndexArray.size() }; }
			};

		};

	};

}

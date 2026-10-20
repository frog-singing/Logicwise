// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>

#include <logicwise/index/type.h>
#include <logicwise/semantics/guard/cycle_detected_in_graph.h>
#include <utility> //用于 std::index_sequence, std:: 
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array
#include <ranges> //用于 std::views，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	struct template_view_adaptor
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
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t NextSize{ Data.size };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

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
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

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
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

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
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

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
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t MaxSize{ Data.size };
				constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }; i < NextSize; ++i)
				{
					next_index_array[i] = Data[i];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------

		//左闭右开区间 [Start, End)
		//此实现等价于 drop<Start>().take<End - Start>() 或 drop<Offset>().take<Count>()
		//不等价于 take<End>().drop<Start>()
		//因为前者有 Start <= End 约束，而后者没有
		template<std::size_t Start, std::size_t End>
		struct slice
		{
			static_assert(Start <= End, "[logicwise] Invalid slice range : Start must be less than or equal to End.");

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };
				constexpr std::size_t CurrentEnd{ Data.size };
				constexpr std::size_t ActualStart{ Start < CurrentEnd ? Start : CurrentEnd };
				constexpr std::size_t ActualEnd{ End < CurrentEnd ? End : CurrentEnd };
				constexpr std::size_t NextSize{ ActualEnd - ActualStart };

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }, j{ ActualStart }; j < ActualEnd; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//================================================================================

		template<typename PredicateSolver>
		struct take_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (!PredicateSolver::template solve<Index>())
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
						if constexpr (!PredicateSolver::template solve<Index>())
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename PredicateSolver>
		struct take_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (PredicateSolver::template solve<Index>())
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
						if constexpr (PredicateSolver::template solve<Index>())
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct drop_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				constexpr std::size_t NextSize = []{

					std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (!PredicateSolver::template solve<Index>())
						{
							break;
						}
						--next_size;
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(... || [&] <auto Index> {
							if constexpr (!PredicateSolver::template solve<Index>())
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

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }, j{ Data.size - NextSize }; i < NextSize; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct drop_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				constexpr std::size_t NextSize = []{

					std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (PredicateSolver::template solve<Index>())
						{
							break;
						}
						--next_size;
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(... || [&] <auto Index> {
							if constexpr (PredicateSolver::template solve<Index>())
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

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }, j{ Data.size - NextSize }; i < NextSize; ++i, ++j)
				{
					next_index_array[i] = Data[j];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct take_last_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_offset{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data | std::views::reverse)
				{
					if constexpr (!PredicateSolver::template solve<Index>())
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
						if constexpr (!PredicateSolver::template solve<Index>())
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct take_last_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_offset{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data | std::views::reverse)
				{
					if constexpr (PredicateSolver::template solve<Index>())
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
						if constexpr (PredicateSolver::template solve<Index>())
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct drop_last_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				constexpr std::size_t NextSize = []{

					std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data | std::views::reverse)
					{
						if constexpr (!PredicateSolver::template solve<Index>())
						{
							break;
						}
						--next_size;
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(... || [&] <auto Index> {
							if constexpr (!PredicateSolver::template solve<Index>())
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

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }; i < NextSize; ++i)
				{
					next_index_array[i] = Data[i];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct drop_last_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				constexpr std::size_t NextSize = []{

					std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data | std::views::reverse)
					{
						if constexpr (PredicateSolver::template solve<Index>())
						{
							break;
						}
						--next_size;
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(... || [&] <auto Index> {
							if constexpr (PredicateSolver::template solve<Index>())
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

				auto next_index_array{ Data.template make_index_array<NextSize>() };

				for (std::size_t i{ 0 }; i < NextSize; ++i)
				{
					next_index_array[i] = Data[i];
				}

				return ViewData{ next_index_array, NextOffset, NextSize };
			}
		};

		//================================================================================
		
		template<typename PredicateSolver>
		struct select
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (PredicateSolver::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (PredicateSolver::template solve<Index>())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PredicateSolver>
		struct remove
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Data.size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (!PredicateSolver::template solve<Index>())
					{
						next_index_array[next_size] = Index;
						++next_size;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (!PredicateSolver::template solve<Index>())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Data.size>{});
#endif

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//================================================================================

		template<typename EquivalenceRelationSolver>
		struct unique_first
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto OuterIndex = Data[I];

					bool should_collect{ true };

					template for (constexpr std::size_t J : std::views::iota(0uz, I))
					{
						constexpr auto RightIndex = Data[J];

						if constexpr (EquivalenceRelationSolver
							::template solve<OuterIndex, RightIndex>())
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
							//添加 (void) 来规避 Clang -Wunused-value 未使用值警告
							(void)(... || [&] <auto RightIndex> {
								if constexpr (EquivalenceRelationSolver
									::template solve<OuterIndex, RightIndex>())
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelationSolver>
		struct unique_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };
				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Size>() };
				std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto OuterIndex = Data[I];

					bool should_collect{ true };

					template for (constexpr std::size_t J : std::views::iota(I + 1, Size) | std::views::reverse)
					{
						constexpr auto RightIndex = Data[J];

						if constexpr (EquivalenceRelationSolver
							::template solve<OuterIndex, RightIndex>())
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
							//添加 (void) 来规避 Clang -Wunused-value 未使用值警告
							(void)(... || [&] <auto RightIndex> {
								if constexpr (EquivalenceRelationSolver
									::template solve<OuterIndex, RightIndex>())
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelationSolver>
		struct unique_adjacent_first
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				if constexpr (Size == 0) { return Data.template make_view_data<0>({}, 0, 0); }
				else
				{
					constexpr std::size_t NextOffset{ 0 };

					auto next_index_array{ Data.template make_index_array<Size>(Data[0]) };
					std::size_t next_size{ 1 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr std::size_t I : std::views::iota(0uz, Size - 1))
					{
						constexpr auto PreviousIndex{ Data[I] };
						constexpr auto CurrentIndex{ Data[I + 1] };

						if constexpr (!EquivalenceRelationSolver
							::template solve<PreviousIndex, CurrentIndex>())
						{
							next_index_array[next_size] = CurrentIndex;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <std::size_t CurrentI> {
							constexpr auto PreviousIndex{ Data[CurrentI] };
							constexpr auto CurrentIndex{ Data[CurrentI + 1] };

							if constexpr (!EquivalenceRelationSolver
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelationSolver>
		struct unique_adjacent_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				if constexpr (Size == 0) { return Data.template make_view_data<0>({}, 0, 0); }
				else
				{
					constexpr std::size_t NextOffset{ 0 };

					auto next_index_array{ Data.template make_index_array<Size>() };
					std::size_t next_size{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr std::size_t I : std::views::iota(0uz, Size - 1))
					{
						constexpr auto CurrentIndex{ Data[I] };
						constexpr auto NextIndex{ Data[I + 1] };

						if constexpr (!EquivalenceRelationSolver
							::template solve<CurrentIndex, NextIndex>())
						{
							next_index_array[next_size] = CurrentIndex;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <std::size_t CurrentI> {
							constexpr auto CurrentIndex{ Data[CurrentI] };
							constexpr auto NextIndex{ Data[CurrentI + 1] };

							if constexpr (!EquivalenceRelationSolver
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
			}
		};

		//================================================================================

		template<typename StrictWeakOrderSolver>
		struct sort
		{
			template<auto Data> requires (Data.size == 0)
			static constexpr auto merge_sort() { return Data.template make_view_data<0>({}, 0, 0); }
			
			template<auto Data> requires (Data.size == 1)
			static constexpr auto merge_sort() { return Data.template make_view_data<1>({ Data[0] }, 0, 1); }

			template<auto Data> requires (Data.size == 2)
			static constexpr auto merge_sort()
			{
				if constexpr (StrictWeakOrderSolver::template solve<Data[1], Data[0]>())
				{
					return Data.template make_view_data<2>({ Data[1], Data[0] }, 0, 2);
				}
				else
				{
					return Data.template make_view_data<2>({ Data[0], Data[1] }, 0, 2);
				}
			}

			template<auto LeftData, auto RightData>
			struct data_merger
			{
				static constexpr std::size_t LeftSize{ LeftData.size };
				static constexpr std::size_t RightSize{ RightData.size };
				static constexpr std::size_t Size{ LeftSize + RightSize };

				template<std::size_t LeftCursor, std::size_t RightCursor>
				static constexpr void merge_at(auto& merged_index_array)
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
						if constexpr (StrictWeakOrderSolver
							::template solve<RightData[RightCursor], LeftData[LeftCursor]>())
						{
							merged_index_array[MergedSize] = RightData[RightCursor];
							merge_at<LeftCursor, RightCursor + 1>(merged_index_array);
						}
						else
						{
							merged_index_array[MergedSize] = LeftData[LeftCursor];
							merge_at<LeftCursor + 1, RightCursor>(merged_index_array);
						}
					}
				}
			};

			template<auto Data>
			static constexpr auto merge_sort()
			{
				constexpr std::size_t Size{ Data.size };
				constexpr std::size_t LeftSize{ Size / 2 };
				constexpr std::size_t RightSize{ Size - LeftSize };

				constexpr auto LeftData = []{

					auto left_index_array{ Data.template make_index_array<LeftSize>() };

					for (std::size_t i{ 0 }; i < LeftSize; ++i)
					{
						left_index_array[i] = Data[i];
					}

					return ViewData{ left_index_array, 0, LeftSize };
				}();

				constexpr auto RightData = []{

					auto right_index_array{ Data.template make_index_array<RightSize>() };

					for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
					{
						right_index_array[i] = Data[j];
					}

					return ViewData{ right_index_array, 0, RightSize };
				}();

				constexpr auto SortedLeftData{ merge_sort<LeftData>() };
				constexpr auto SortedRightData{ merge_sort<RightData>() };

				auto merged_index_array{ Data.template make_index_array<Size>() };

				data_merger<SortedLeftData, SortedRightData>::template merge_at<0, 0>(merged_index_array);

				return ViewData{ merged_index_array, 0, Size };
			}

			template<auto Data>
			static constexpr auto adapt()
			{
				return merge_sort<Data>();
			}
		};

		//================================================================================

		template<typename PartialWeakOrderSolver>
		struct sort_with_partial_weak_order
		{
			template<auto Data> requires (Data.size == 0)
			static constexpr auto merge_sort() { return Data.template make_view_data<0>({}, 0, 0); }

			template<auto Data> requires (Data.size == 1)
			static constexpr auto merge_sort() { return Data.template make_view_data<1>({ Data[0] }, 0, 1); }
			
			template<auto Data> requires (Data.size == 2)
			static constexpr auto merge_sort()
			{
				if constexpr (PartialWeakOrderSolver::template solve<Data[0], Data[1]>())
				{
					return Data.template make_view_data<2>({ Data[0], Data[1] }, 0, 2);
				}
				else
				{
					return Data.template make_view_data<2>({ Data[1], Data[0] }, 0, 2);
				}
			}

			template<auto LeftData, auto RightData>
			struct data_merger
			{
				static constexpr std::size_t LeftSize{ LeftData.size };
				static constexpr std::size_t RightSize{ RightData.size };
				static constexpr std::size_t Size{ LeftSize + RightSize };

				template<std::size_t LeftCursor, std::size_t RightCursor>
				static constexpr void merge_at(auto& merged_index_array)
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
						if constexpr (PartialWeakOrderSolver
							::template solve<LeftData[LeftCursor], RightData[RightCursor]>())
						{
							merged_index_array[MergedSize] = LeftData[LeftCursor];
							merge_at<LeftCursor + 1, RightCursor>(merged_index_array);
						}
						else
						{
							merged_index_array[MergedSize] = RightData[RightCursor];
							merge_at<LeftCursor, RightCursor + 1>(merged_index_array);
						}
					}
				}
			};

			template<auto Data>
			static constexpr auto merge_sort()
			{
				constexpr std::size_t Size{ Data.size };
				constexpr std::size_t LeftSize{ Size / 2 };
				constexpr std::size_t RightSize{ Size - LeftSize };

				constexpr auto LeftData = []{

					auto left_index_array{ Data.template make_index_array<LeftSize>() };

					for (std::size_t i{ 0 }; i < LeftSize; ++i)
					{
						left_index_array[i] = Data[i];
					}

					return ViewData{ left_index_array, 0, LeftSize };
				}();

				constexpr auto RightData = []{

					auto right_index_array{ Data.template make_index_array<RightSize>() };

					for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
					{
						right_index_array[i] = Data[j];
					}

					return ViewData{ right_index_array, 0, RightSize };
				}();

				constexpr auto SortedLeftData{ merge_sort<LeftData>() };
				constexpr auto SortedRightData{ merge_sort<RightData>() };

				auto merged_index_array{ Data.template make_index_array<Size>() };

				data_merger<SortedLeftData, SortedRightData>::template merge_at<0, 0>(merged_index_array);

				return ViewData{ merged_index_array, 0, Size };
			}

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				constexpr auto ReflexiveData = []{

					auto reflexive_index_array{ Data.template make_index_array<Size>() };
					std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
						{
							reflexive_index_array[reflexive_size] = Index;
							++reflexive_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
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

				auto sorted_index_array{ Data.template make_index_array<Size>() };
				std::size_t irreflexive_cursor{ ReflexiveSize };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
					{
						sorted_index_array[irreflexive_cursor] = Index;
						++irreflexive_cursor;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
						{
							sorted_index_array[irreflexive_cursor] = Index;
							++irreflexive_cursor;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				constexpr auto SortedReflexiveData{ merge_sort<ReflexiveData>() };

				for (std::size_t i{ 0 }; i < ReflexiveSize; ++i)
				{
					sorted_index_array[i] = SortedReflexiveData[i];
				}

				return ViewData{ sorted_index_array, 0, Size };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PartialWeakOrderSolver>
		struct unique_first_with_partial_weak_order
		{
			template<auto Data>
			struct position_data_sorter
			{
				template<auto PositionData> requires (PositionData.size == 0)
				static constexpr auto merge_sort() { return ViewData<std::size_t, 0>{ {}, 0, 0 }; }

				template<auto PositionData> requires (PositionData.size == 1)
				static constexpr auto merge_sort() { return ViewData<std::size_t, 1>{ { PositionData[0] }, 0, 1 }; }

				template<auto PositionData> requires (PositionData.size == 2)
				static constexpr auto merge_sort()
				{
					if constexpr (PartialWeakOrderSolver
						::template solve<Data[PositionData[0]], Data[PositionData[1]]>())
					{
						return ViewData<std::size_t, 2>{ { PositionData[0], PositionData[1] }, 0, 2 };
					}
					else
					{
						return ViewData<std::size_t, 2>{ { PositionData[1], PositionData[0] }, 0, 2 };
					}
				}

				template<auto LeftPositionData, auto RightPositionData>
				struct position_data_merger
				{
					static constexpr std::size_t LeftSize{ LeftPositionData.size };
					static constexpr std::size_t RightSize{ RightPositionData.size };
					static constexpr std::size_t Size{ LeftSize + RightSize };

					template<std::size_t LeftCursor, std::size_t RightCursor>
					static constexpr void merge_at(std::array<std::size_t, Size>& merged_position_array)
					{
						constexpr std::size_t MergedSize{ LeftCursor + RightCursor };

						if constexpr (LeftCursor == LeftSize)
						{
							for (std::size_t i{ MergedSize }, j{ RightCursor }; j < RightSize; ++i, ++j)
							{
								merged_position_array[i] = RightPositionData[j];
							}
						}
						else if constexpr (RightCursor == RightSize)
						{
							for (std::size_t i{ MergedSize }, j{ LeftCursor }; j < LeftSize; ++i, ++j)
							{
								merged_position_array[i] = LeftPositionData[j];
							}
						}
						else
						{
							if constexpr (PartialWeakOrderSolver
								::template solve<Data[LeftPositionData[LeftCursor]], Data[RightPositionData[RightCursor]]>())
							{
								merged_position_array[MergedSize] = LeftPositionData[LeftCursor];
								merge_at<LeftCursor + 1, RightCursor>(merged_position_array);
							}
							else
							{
								merged_position_array[MergedSize] = RightPositionData[RightCursor];
								merge_at<LeftCursor, RightCursor + 1>(merged_position_array);
							}
						}
					}
				};

				template<auto PositionData>
				static constexpr auto merge_sort()
				{
					constexpr std::size_t Size{ PositionData.size };
					constexpr std::size_t LeftSize{ Size / 2 };
					constexpr std::size_t RightSize{ Size - LeftSize };

					constexpr auto LeftPositionData = []{

						std::array<std::size_t, LeftSize> left_position_array{};

						for (std::size_t i{ 0 }; i < LeftSize; ++i)
						{
							left_position_array[i] = PositionData[i];
						}

						return ViewData{ left_position_array, 0, LeftSize };
					}();

					constexpr auto RightPositionData = []{

						std::array<std::size_t, RightSize> right_position_array{};

						for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
						{
							right_position_array[i] = PositionData[j];
						}

						return ViewData{ right_position_array, 0, RightSize };
					}();

					constexpr auto SortedLeftPositionData{ merge_sort<LeftPositionData>() };
					constexpr auto SortedRightPositionData{ merge_sort<RightPositionData>() };

					std::array<std::size_t, Size> merged_position_array{};

					position_data_merger<SortedLeftPositionData, SortedRightPositionData>
						::template merge_at<0, 0>(merged_position_array);

					return ViewData{ merged_position_array, 0, Size };
				}
			};

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				constexpr auto ReflexivePositionData = []{

					std::array<std::size_t, Size> reflexive_position_array{};
					std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr std::size_t I : std::views::iota(0uz, Size))
					{
						constexpr auto Index{ Data[I] };

						if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
						{
							reflexive_position_array[reflexive_size] = I;
							++reflexive_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
							{
								reflexive_position_array[reflexive_size] = I;
								++reflexive_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Size>{});
#endif

					return ViewData{ reflexive_position_array, 0, reflexive_size };
				}();

				constexpr std::size_t ReflexiveSize{ ReflexivePositionData.size };

				if constexpr (ReflexiveSize == 0) { return Data; }

				std::array<bool, Size> should_collect_map{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					should_collect_map[i] = false;
				}

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto Index{ Data[I] };

					if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
					{
						should_collect_map[I] = true;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
						{
							should_collect_map[I] = true;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				constexpr auto SortedReflexivePositionData
				{
					position_data_sorter<Data>::template merge_sort<ReflexivePositionData>()
				};

				should_collect_map[SortedReflexivePositionData[0]] = true;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, ReflexiveSize - 1))
				{
					constexpr auto PreviousIndex{ Data[SortedReflexivePositionData[I]] };
					constexpr auto CurrentPosition{ SortedReflexivePositionData[I + 1] };
					constexpr auto CurrentIndex{ Data[CurrentPosition] };

					if constexpr (!PartialWeakOrderSolver::template solve<CurrentIndex, PreviousIndex>())
					{
						should_collect_map[CurrentPosition] = true;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <std::size_t CurrentI> {
						constexpr auto PreviousIndex{ Data[SortedReflexivePositionData[CurrentI]] };
						constexpr auto CurrentPosition{ SortedReflexivePositionData[CurrentI + 1] };
						constexpr auto CurrentIndex{ Data[CurrentPosition] };

						if constexpr (!PartialWeakOrderSolver::template solve<CurrentIndex, PreviousIndex>())
						{
							should_collect_map[CurrentPosition] = true;
						}
					}.template operator() < I > ());
				}(std::make_index_sequence<ReflexiveSize - 1>{});
#endif

				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Size>() };
				std::size_t next_size{ 0 };

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					if (should_collect_map[i])
					{
						next_index_array[next_size] = Data[i];
						++next_size;
					}
				}

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PartialWeakOrderSolver>
		struct unique_last_with_partial_weak_order
		{
			template<auto Data>
			struct position_data_sorter
			{
				template<auto PositionData> requires (PositionData.size == 0)
				static constexpr auto merge_sort() { return ViewData<std::size_t, 0>{ {}, 0, 0 }; }

				template<auto PositionData> requires (PositionData.size == 1)
				static constexpr auto merge_sort() { return ViewData<std::size_t, 1>{ { PositionData[0] }, 0, 1 }; }

				template<auto PositionData> requires (PositionData.size == 2)
				static constexpr auto merge_sort()
				{
					if constexpr (PartialWeakOrderSolver::template solve<Data[PositionData[0]], Data[PositionData[1]]>())
					{
						return ViewData<std::size_t, 2>{ { PositionData[0], PositionData[1] }, 0, 2 };
					}
					else
					{
						return ViewData<std::size_t, 2>{ { PositionData[1], PositionData[0] }, 0, 2 };
					}
				}

				template<auto LeftPositionData, auto RightPositionData>
				struct position_data_merger
				{
					static constexpr std::size_t LeftSize{ LeftPositionData.size };
					static constexpr std::size_t RightSize{ RightPositionData.size };
					static constexpr std::size_t Size{ LeftSize + RightSize };

					template<std::size_t LeftCursor, std::size_t RightCursor>
					static constexpr void merge_at(std::array<std::size_t, Size>& merged_position_array)
					{
						constexpr std::size_t MergedSize{ LeftCursor + RightCursor };

						if constexpr (LeftCursor == LeftSize)
						{
							for (std::size_t i{ MergedSize }, j{ RightCursor }; j < RightSize; ++i, ++j)
							{
								merged_position_array[i] = RightPositionData[j];
							}
						}
						else if constexpr (RightCursor == RightSize)
						{
							for (std::size_t i{ MergedSize }, j{ LeftCursor }; j < LeftSize; ++i, ++j)
							{
								merged_position_array[i] = LeftPositionData[j];
							}
						}
						else
						{
							if constexpr (PartialWeakOrderSolver
								::template solve<Data[LeftPositionData[LeftCursor]], Data[RightPositionData[RightCursor]]>())
							{
								merged_position_array[MergedSize] = LeftPositionData[LeftCursor];
								merge_at<LeftCursor + 1, RightCursor>(merged_position_array);
							}
							else
							{
								merged_position_array[MergedSize] = RightPositionData[RightCursor];
								merge_at<LeftCursor, RightCursor + 1>(merged_position_array);
							}
						}
					}
				};

				template<auto PositionData>
				static constexpr auto merge_sort()
				{
					constexpr std::size_t Size{ PositionData.size };
					constexpr std::size_t LeftSize{ Size / 2 };
					constexpr std::size_t RightSize{ Size - LeftSize };

					constexpr auto LeftPositionData = []{

						std::array<std::size_t, LeftSize> left_position_array{};

						for (std::size_t i{ 0 }; i < LeftSize; ++i)
						{
							left_position_array[i] = PositionData[i];
						}

						return ViewData{ left_position_array, 0, LeftSize };
					}();

					constexpr auto RightPositionData = []{

						std::array<std::size_t, RightSize> right_position_array{};

						for (std::size_t i{ 0 }, j{ LeftSize }; i < RightSize; ++i, ++j)
						{
							right_position_array[i] = PositionData[j];
						}

						return ViewData{ right_position_array, 0, RightSize };
					}();

					constexpr auto SortedLeftPositionData{ merge_sort<LeftPositionData>() };
					constexpr auto SortedRightPositionData{ merge_sort<RightPositionData>() };

					std::array<std::size_t, Size> merged_position_array{};

					position_data_merger<SortedLeftPositionData, SortedRightPositionData>
						::template merge_at<0, 0>(merged_position_array);

					return ViewData{ merged_position_array, 0, Size };
				}
			};

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				constexpr auto ReflexivePositionData = []{

					std::array<std::size_t, Size> reflexive_position_array{};
					std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr std::size_t I : std::views::iota(0uz, Size))
					{
						constexpr auto Index{ Data[I] };

						if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
						{
							reflexive_position_array[reflexive_size] = I;
							++reflexive_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (PartialWeakOrderSolver::template solve<Index, Index>())
							{
								reflexive_position_array[reflexive_size] = I;
								++reflexive_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Size>{});
#endif

					return ViewData{ reflexive_position_array, 0, reflexive_size };
				}();

				constexpr std::size_t ReflexiveSize{ ReflexivePositionData.size };

				if constexpr (ReflexiveSize == 0) { return Data; }

				std::array<bool, Size> should_collect_map{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					should_collect_map[i] = false;
				}

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto Index{ Data[I] };

					if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
					{
						should_collect_map[I] = true;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (!PartialWeakOrderSolver::template solve<Index, Index>())
						{
							should_collect_map[I] = true;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				constexpr auto SortedReflexivePositionData
				{
					position_data_sorter<Data>::template merge_sort<ReflexivePositionData>()
				};

				should_collect_map[SortedReflexivePositionData[ReflexiveSize - 1]] = true;

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, ReflexiveSize - 1))
				{
					constexpr auto CurrentPosition{ SortedReflexivePositionData[I] };
					constexpr auto CurrentIndex{ Data[CurrentPosition] };
					constexpr auto NextIndex{ Data[SortedReflexivePositionData[I + 1]] };

					if constexpr (!PartialWeakOrderSolver::template solve<NextIndex, CurrentIndex>())
					{
						should_collect_map[CurrentPosition] = true;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <std::size_t CurrentI> {
						constexpr auto CurrentPosition{ SortedReflexivePositionData[CurrentI] };
						constexpr auto CurrentIndex{ Data[CurrentPosition] };
						constexpr auto NextIndex{ Data[SortedReflexivePositionData[CurrentI + 1]] };

						if constexpr (!PartialWeakOrderSolver::template solve<NextIndex, CurrentIndex>())
						{
							should_collect_map[CurrentPosition] = true;
						}
					}.template operator() < I > ());
				}(std::make_index_sequence<ReflexiveSize - 1>{});
#endif

				constexpr std::size_t NextOffset{ 0 };

				auto next_index_array{ Data.template make_index_array<Size>() };
				std::size_t next_size{ 0 };

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					if (should_collect_map[i])
					{
						next_index_array[next_size] = Data[i];
						++next_size;
					}
				}

				return ViewData{ next_index_array, NextOffset, next_size };
			}
		};

		//================================================================================

		template<typename AcyclicRelationSolver>
		struct sort_topologically
		{
			template<auto Data>
			static constexpr auto kahn_topological_sort()
			{
				constexpr std::size_t Size{ Data.size };

				std::array<std::array<bool, Size>, Size> relation_matrix{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto LeftIndex{ Data[I] };
					auto& relation_matrix_row{ relation_matrix[I] };

					template for (constexpr std::size_t J : std::views::iota(0uz, Size))
					{
						constexpr auto RightIndex{ Data[J] };

						relation_matrix_row[J] =
							AcyclicRelationSolver::template solve<LeftIndex, RightIndex>();
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto LeftIndex> {
						auto& relation_matrix_row{ relation_matrix[I] };

						[&] <std::size_t... J> (std::index_sequence<J...>) {
							(..., [&] <auto RightIndex> {
								relation_matrix_row[J] =
									AcyclicRelationSolver::template solve<LeftIndex, RightIndex>();
							}.template operator() < Data[J] > ());
						}(std::make_index_sequence<Size>{});
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				std::array<std::size_t, Size> predecessor_count_map{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					const auto& relation_matrix_row{ relation_matrix[i] };

					for (std::size_t j{ 0 }; j < Size; ++j)
					{
						if (relation_matrix_row[j]) { ++predecessor_count_map[j]; }
					}
				}

				std::size_t processed_count{ 0 };
				std::size_t sorted_count{ 0 };
				std::array<std::size_t, Size> processing_queue{};
				auto sorted_index_array{ Data.template make_index_array<Size>() };

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					if (predecessor_count_map[i] == 0)
					{
						sorted_index_array[sorted_count] = Data[i];
						processing_queue[sorted_count] = i;
						++sorted_count;
					}
				}

				while (sorted_count < Size)
				{
					const auto current_element{ processing_queue[processed_count] };
					const auto& current_relation_matrix_row{ relation_matrix[current_element] };

					for (std::size_t i{ 0 }; i < Size; ++i)
					{
						if (current_relation_matrix_row[i])
						{
							auto& predecessor_count{ predecessor_count_map[i] };
							--predecessor_count;

							if (predecessor_count == 0)
							{
								sorted_index_array[sorted_count] = Data[i];
								processing_queue[sorted_count] = i;
								++sorted_count;
							}
						}
					}

					++processed_count;

					if (processed_count >= sorted_count) [[unlikely]]
					{
						cycle_detected_in_graph();
					}
				}

				return ViewData{ sorted_index_array, 0, Size };
			}

			template<auto Data>
			static constexpr auto adapt()
			{
				return kahn_topological_sort<Data>();
			}
		};

		//================================================================================

		template<typename PartialPartialOrderSolver>
		struct sort_topologically_with_partial_partial_order
		{
			template<auto Data>
			static constexpr auto kahn_topological_sort()
			{
				constexpr std::size_t Size{ Data.size };

				std::array<std::array<bool, Size>, Size> relation_matrix{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto LeftIndex{ Data[I] };
					auto& relation_matrix_row{ relation_matrix[I] };

					template for (constexpr std::size_t J : std::views::iota(0uz, Size))
					{
						constexpr auto RightIndex{ Data[J] };

						if constexpr (I != J)
						{
							relation_matrix_row[J] =
								PartialPartialOrderSolver::template solve<LeftIndex, RightIndex>();
						}
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto LeftIndex> {
						constexpr auto CurrentI{ I };
						auto& relation_matrix_row{ relation_matrix[CurrentI] };

						[&] <std::size_t... J> (std::index_sequence<J...>) {
							(..., [&] <auto RightIndex> {
								constexpr auto CurrentJ{ J };

								if constexpr (CurrentI != CurrentJ)
								{
									relation_matrix_row[CurrentJ] =
										PartialPartialOrderSolver::template solve<LeftIndex, RightIndex>();
								}
							}.template operator() < Data[J] > ());
						}(std::make_index_sequence<Size>{});
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				std::array<std::size_t, Size> representative_map{};
				std::array<std::size_t, Size> quotient_set_tail_element_map{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					representative_map[i] = i;
					quotient_set_tail_element_map[i] = Size;
				}

				for (std::size_t i{ 0 }; i < Size - 1; ++i)
				{
					const auto representative{ representative_map[i] };
					auto& relation_matrix_row{ relation_matrix[i] };

					for (std::size_t j{ i + 1 }; j < Size; ++j)
					{
						auto& relation{ relation_matrix_row[j] };
						auto& dual_relation{ relation_matrix[j][i] };

						if (relation && dual_relation)
						{
							auto& representative_slot{ representative_map[j] };

							if (representative_slot != representative)
							{
								representative_slot = representative;
							}

							relation = false;
							dual_relation = false;
						}
					}
				}

				std::array<std::size_t, Size> representative_array{};
				std::size_t representative_size{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					if (representative_map[i] == i)
					{
						representative_array[representative_size] = i;
						++representative_size;
					}
				}

				std::array<std::size_t, Size> equivalence_class_next_element_map{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					const auto representative{ representative_map[i] };
					auto& tail_element{ quotient_set_tail_element_map[representative] };

					if (tail_element < Size)
					{
						equivalence_class_next_element_map[tail_element] = i;
					}

					tail_element = i;
				}

				std::array<std::size_t, Size> representative_predecessor_count_array{};

				for (std::size_t i{ 0 }; i < representative_size; ++i)
				{
					const auto representative_i{ representative_array[i] };
					const auto& relation_matrix_row{ relation_matrix[representative_i] };

					for (std::size_t j{ 0 }; j < representative_size; ++j)
					{
						const auto representative_j{ representative_array[j] };

						if (relation_matrix_row[representative_j])
						{
							++representative_predecessor_count_array[j];
						}
					}
				}

				std::size_t processed_count{ 0 };
				std::size_t sorted_count{ 0 };
				std::array<std::size_t, Size> processing_queue{};

				for (std::size_t i{ 0 }; i < representative_size; ++i)
				{
					if (representative_predecessor_count_array[i] == 0)
					{
						processing_queue[sorted_count] = representative_array[i];
						++sorted_count;
					}
				}

				while (sorted_count < representative_size)
				{
					const auto current_representative{ processing_queue[processed_count] };
					const auto& current_relation_matrix_row{ relation_matrix[current_representative] };

					for (std::size_t i{ 0 }; i < representative_size; ++i)
					{
						const auto representative_i{ representative_array[i] };

						if (current_relation_matrix_row[representative_i])
						{
							auto& predecessor_count{ representative_predecessor_count_array[i] };
							--predecessor_count;

							if (predecessor_count == 0)
							{
								processing_queue[sorted_count] = representative_i;
								++sorted_count;
							}
						}
					}

					++processed_count;

					if (processed_count >= sorted_count) [[unlikely]]
					{
						cycle_detected_in_graph();
					}
				}

				auto sorted_index_array{ Data.template make_index_array<Size>() };
				std::size_t sorted_index_count{ 0 };

				for (std::size_t i{ 0 }; i < representative_size; ++i)
				{
					const auto representative{ processing_queue[i] };
					const auto tail_element{ quotient_set_tail_element_map[representative] };
					auto current_element{ representative };

					while (true)
					{
						sorted_index_array[sorted_index_count] = Data[current_element];
						++sorted_index_count;

						if (current_element == tail_element) { break; }

						current_element = equivalence_class_next_element_map[current_element];
					}
				}

				return ViewData{ sorted_index_array, 0, Size };
			}

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				constexpr auto ReflexiveData = []{

					auto reflexive_index_array{ Data.template make_index_array<Size>() };
					std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (PartialPartialOrderSolver::template solve<Index, Index>())
						{
							reflexive_index_array[reflexive_size] = Index;
							++reflexive_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (PartialPartialOrderSolver::template solve<Index, Index>())
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

				auto sorted_index_array{ Data.template make_index_array<Size>() };
				std::size_t irreflexive_cursor{ ReflexiveSize };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr auto Index : Data)
				{
					if constexpr (!PartialPartialOrderSolver::template solve<Index, Index>())
					{
						sorted_index_array[irreflexive_cursor] = Index;
						++irreflexive_cursor;
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto Index> {
						if constexpr (!PartialPartialOrderSolver::template solve<Index, Index>())
						{
							sorted_index_array[irreflexive_cursor] = Index;
							++irreflexive_cursor;
						}
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				constexpr auto SortedReflexiveData{ kahn_topological_sort<ReflexiveData>() };

				for (std::size_t i{ 0 }; i < ReflexiveSize; ++i)
				{
					sorted_index_array[i] = SortedReflexiveData[i];
				}

				return ViewData{ sorted_index_array, 0, Size };
			}
		};

	};

}

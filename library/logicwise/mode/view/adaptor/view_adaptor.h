// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>

#include <logicwise/index/type.h>
#include <logicwise/semantics/guard/cycle_detected_in_graph.h>
#include <utility> //用于 std::index_sequence, std::make_index_sequence
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array
#include <ranges> //用于 std::ranges, std::views，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================
	
	template<typename MaterialAgent>
	struct view_adaptor
	{
		using index_type = typename MaterialAgent::index_type;

		template<typename Predicate>
		using predicate_solver = typename MaterialAgent::template predicate_solver<Predicate>;

		template<typename Predicate>
		using predicate_with_index_solver = typename MaterialAgent::template predicate_with_index_solver<Predicate>;

		template<typename HomogeneousRelation>
		using homogeneous_relation_solver = typename MaterialAgent::template homogeneous_relation_solver<HomogeneousRelation>;

		//================================================================================

		template<typename Predicate>
		struct take_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		struct take_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
		struct take_last_while
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
		struct take_last_until
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
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
			}
		};

		//================================================================================
		
		template<typename Predicate>
		struct select
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename Predicate>
		struct remove
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		struct select_with_index
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		struct remove_with_index
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//================================================================================

		template<typename EquivalenceRelation>
		struct unique_first
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
						constexpr auto RightIndex = Data[J];

						if constexpr (homogeneous_relation_solver<EquivalenceRelation>
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
							(... || [&] <auto RightIndex> {
								if constexpr (homogeneous_relation_solver<EquivalenceRelation>
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

		template<typename EquivalenceRelation>
		struct unique_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
						constexpr auto RightIndex = Data[J];

						if constexpr (homogeneous_relation_solver<EquivalenceRelation>
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
							(... || [&] <auto RightIndex> {
								if constexpr (homogeneous_relation_solver<EquivalenceRelation>
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

		template<typename EquivalenceRelation>
		struct unique_adjacent_first
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
		struct unique_adjacent_last
		{
			template<auto Data>
			static constexpr auto adapt()
			{
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
			}
		};

		//================================================================================

		template<typename StrictWeakOrder>
		struct sort
		{
			template<auto Data> requires (Data.size == 0)
			static constexpr auto merge_sort() { return ViewData<index_type, 0>{ {}, 0, 0 }; }

			template<auto Data> requires (Data.size == 1)
			static constexpr auto merge_sort() { return ViewData<index_type, 1>{ { Data[0] }, 0, 1 }; }

			template<auto Data> requires (Data.size == 2)
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

			template<auto LeftData, auto RightData>
			struct data_merger
			{
				static constexpr std::size_t LeftSize{ LeftData.size };
				static constexpr std::size_t RightSize{ RightData.size };
				static constexpr std::size_t Size{ LeftSize + RightSize };

				template<std::size_t LeftCursor, std::size_t RightCursor>
				static constexpr void merge_at(std::array<index_type, Size>& merged_index_array)
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

				constexpr auto SortedLeftData{ merge_sort<LeftData>() };
				constexpr auto SortedRightData{ merge_sort<RightData>() };

				std::array<index_type, Size> merged_index_array{};

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

		template<typename PartialWeakOrder>
		struct sort_with_partial_weak_order
		{
			template<auto Data> requires (Data.size == 0)
			static constexpr auto merge_sort() { return ViewData<index_type, 0>{ {}, 0, 0 }; }

			template<auto Data> requires (Data.size == 1)
			static constexpr auto merge_sort() { return ViewData<index_type, 1>{ { Data[0] }, 0, 1 }; }

			template<auto Data> requires (Data.size == 2)
			static constexpr auto merge_sort()
			{
				if constexpr (homogeneous_relation_solver<PartialWeakOrder>
					::template solve<Data[0], Data[1]>())
				{
					return ViewData<index_type, 2>{ { Data[0], Data[1] }, 0, 2 };
				}
				else
				{
					return ViewData<index_type, 2>{ { Data[1], Data[0] }, 0, 2 };
				}
			}

			template<auto LeftData, auto RightData>
			struct data_merger
			{
				static constexpr std::size_t LeftSize{ LeftData.size };
				static constexpr std::size_t RightSize{ RightData.size };
				static constexpr std::size_t Size{ LeftSize + RightSize };

				template<std::size_t LeftCursor, std::size_t RightCursor>
				static constexpr void merge_at(std::array<index_type, Size>& merged_index_array)
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
						if constexpr (homogeneous_relation_solver<PartialWeakOrder>
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

				constexpr auto SortedLeftData{ merge_sort<LeftData>() };
				constexpr auto SortedRightData{ merge_sort<RightData>() };

				std::array<index_type, Size> merged_index_array{};

				data_merger<SortedLeftData, SortedRightData>::template merge_at<0, 0>(merged_index_array);

				return ViewData{ merged_index_array, 0, Size };
			}

			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				constexpr auto ReflexiveData = []{

					std::array<index_type, Size> reflexive_index_array{};
					std::size_t reflexive_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (homogeneous_relation_solver<PartialWeakOrder>
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
							if constexpr (homogeneous_relation_solver<PartialWeakOrder>
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
					if constexpr (!homogeneous_relation_solver<PartialWeakOrder>
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
						if constexpr (!homogeneous_relation_solver<PartialWeakOrder>
							::template solve<Index, Index>())
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
		
		template<typename PartialWeakOrder>
		struct unique_first_with_partial_weak_order
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				return Data;
			}
		};

		//--------------------------------------------------------------------------------
		
		template<typename PartialWeakOrder>
		struct unique_last_with_partial_weak_order
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				return Data;
			}
		};

		//================================================================================

		template<typename StrictPartialOrder>
		struct sort_topologically_with_strict_partial_order
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				constexpr std::size_t Size{ Data.size };

				std::array<std::array<bool, Size>, Size> relation_matrix{};

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
				//C++26
				template for (constexpr std::size_t I : std::views::iota(0uz, Size))
				{
					constexpr auto LeftIndex = Data[I];
					auto& relation_matrix_row = relation_matrix[I];

					template for (constexpr std::size_t J : std::views::iota(0uz, Size))
					{
						constexpr auto RightIndex = Data[J];

						relation_matrix_row[J] =
							homogeneous_relation_solver<StrictPartialOrder>
							::template solve<LeftIndex, RightIndex>();
					}
				}
#else
				//C++20
				[&] <std::size_t... I> (std::index_sequence<I...>) {
					(..., [&] <auto LeftIndex> {
						auto& relation_matrix_row = relation_matrix[I];

						[&] <std::size_t... J> (std::index_sequence<J...>) {
							(..., [&] <auto RightIndex> {
								relation_matrix_row[J] =
									homogeneous_relation_solver<StrictPartialOrder>
									::template solve<LeftIndex, RightIndex>();
							}.template operator() < Data[J] > ());
						}(std::make_index_sequence<Size>{});
					}.template operator() < Data[I] > ());
				}(std::make_index_sequence<Size>{});
#endif

				std::array<std::size_t, Size> predecessor_count_array{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					const auto& relation_matrix_row{ relation_matrix[i] };

					for (std::size_t j{ 0 }; j < Size; ++j)
					{
						if (relation_matrix_row[j]) { ++predecessor_count_array[j]; }
					}
				}

				std::size_t processed_count{ 0 };
				std::size_t sorted_count{ 0 };
				std::array<std::size_t, Size> position_array{};
				std::array<index_type, Size> sorted_index_array{};

				for (std::size_t i{ 0 }; i < Size; ++i)
				{
					if (predecessor_count_array[i] == 0)
					{
						sorted_index_array[sorted_count] = Data[i];
						position_array[sorted_count] = i;
						++sorted_count;
					}
				}

				while (sorted_count < Size)
				{
					std::size_t current_position{ position_array[processed_count] };
					const auto& current_relation_matrix_row{ relation_matrix[current_position] };

					for (std::size_t i{ 0 }; i < Size; ++i)
					{
						if (current_relation_matrix_row[i])
						{
							auto& predecessor_count{ predecessor_count_array[i] };
							--predecessor_count;

							if (predecessor_count == 0)
							{
								sorted_index_array[sorted_count] = Data[i];
								position_array[sorted_count] = i;
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
		};

		//================================================================================

		template<typename PartialPartialOrder>
		struct sort_topologically_with_partial_partial_order
		{
			template<auto Data>
			static constexpr auto adapt()
			{
				return Data;
			}
		};

	};

}

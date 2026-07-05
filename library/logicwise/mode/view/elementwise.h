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
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	struct elementwise_view;

	template<typename Mode, typename Arrangement>
	class elementwise_viewing;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================
	
	struct elementwise_view
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		static constexpr auto identity = [] <auto Data> { return Data; };

		template<typename PreviousView, auto ViewAdaptor = identity>
		struct type_list_view
		{
			using list = typename PreviousView::list;
			
			struct private_data
			{
				static constexpr auto data{ ViewAdaptor.template operator() < PreviousView::private_data::get_data() > () };
				static constexpr auto get_data() noexcept { return data; }
			};

			using CurrentView = type_list_view;


			static constexpr auto reverse()
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };
					constexpr std::size_t NextSize{ Data.size };

					std::array<index_type, NextSize> next_index_array{};

					for (std::size_t i{ 0 }, j{ NextSize - 1 }; i < NextSize; ++i, --j)
					{
						next_index_array[i] = Data[j];
					}

					return ViewData{ next_index_array, NextOffset, NextSize };
				} > {};
			}


			template<std::size_t Count>
			static constexpr auto take()
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };
					constexpr std::size_t MaxSize{ Data.size };
					constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

					std::array<index_type, NextSize> next_index_array{};

					for (std::size_t i{ 0 }; i < NextSize; ++i)
					{
						next_index_array[i] = Data[i];
					}

					return ViewData{ next_index_array, NextOffset, NextSize };
				} > {};
			}

			template<std::size_t Count>
			static constexpr auto drop()
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };
					constexpr std::size_t MaxSize{ Data.size };
					constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

					std::array<index_type, NextSize> next_index_array{};

					for (std::size_t i{ 0 }, j{ Count }; i < NextSize; ++i, ++j)
					{
						next_index_array[i] = Data[j];
					}

					return ViewData{ next_index_array, NextOffset, NextSize };
				} > {};
			}

			template<std::size_t Count>
			static constexpr auto take_last()
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };
					constexpr std::size_t MaxSize{ Data.size };
					constexpr std::size_t NextSize{ Count < MaxSize ? Count : MaxSize };

					std::array<index_type, NextSize> next_index_array{};

					for (std::size_t i{ 0 }, j{ MaxSize - NextSize }; i < NextSize; ++i, ++j)
					{
						next_index_array[i] = Data[j];
					}

					return ViewData{ next_index_array, NextOffset, NextSize };
				} > {};
			}

			template<std::size_t Count>
			static constexpr auto drop_last()
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };
					constexpr std::size_t MaxSize{ Data.size };
					constexpr std::size_t NextSize{ Count < MaxSize ? MaxSize - Count : 0 };

					std::array<index_type, NextSize> next_index_array{};

					for (std::size_t i{ 0 }; i < NextSize; ++i)
					{
						next_index_array[i] = Data[i];
					}

					return ViewData{ next_index_array, NextOffset, NextSize };
				} > {};
			}

			//左闭右开区间，[Start, End)
			//此实现等价于 drop<Start>().take<End - Start>() 或 drop<Offset>().take<Count>()
			//不等价于 take<End>().drop<Start>()
			template<std::size_t Start, std::size_t End>
			static constexpr auto slice()
			{
				static_assert(Start <= End, "[logicwise] Invalid slice range : Start must be less than or equal to End.");

				return type_list_view < CurrentView, [] <auto Data> {

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
				} > {};
			}


			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto take_while(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
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
							if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
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
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto take_until(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
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
							if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
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
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto drop_while(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					constexpr std::size_t NextSize = []{
						
						std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
						//C++26
						template for (constexpr auto Index : Data)
						{
							if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
							{
								break;
							}
							--next_size;
						}
#else
						//C++20
						[&] <std::size_t... I> (std::index_sequence<I...>) {
							(... || [&] <auto Index> {
								if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
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
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto drop_until(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					constexpr std::size_t NextSize = []{
						
						std::size_t next_size{ Data.size };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
						//C++26
						template for (constexpr auto Index : Data)
						{
							if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
							{
								break;
							}
							--next_size;
						}
#else
						//C++20
						[&] <std::size_t... I> (std::index_sequence<I...>) {
							(... || [&] <auto Index> {
								if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
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
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[private_data::data.size - 1]> > () };
			}
			static constexpr auto take_last_while(Predicate&&)
			{

			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[private_data::data.size - 1]> > () };
			}
			static constexpr auto take_last_until(Predicate&&)
			{

			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[private_data::data.size - 1]> > () };
			}
			static constexpr auto drop_last_while(Predicate&&)
			{

			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[private_data::data.size - 1]> > () };
			}
			static constexpr auto drop_last_until(Predicate&&)
			{

			}


			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto select(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (Predicate{}.template operator() < typename list::template element<Index> > ())
							{
								next_index_array[next_size] = Index;
								++next_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Data.size>{});
#endif

					return ViewData{ next_index_array, NextOffset, next_size };
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto remove(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (!Predicate{}.template operator() < typename list::template element<Index> > ())
							{
								next_index_array[next_size] = Index;
								++next_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Data.size>{});
#endif

					return ViewData{ next_index_array, NextOffset, next_size };
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< private_data::data[0], typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto select_with_index(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (Predicate{}.template operator() < Index, typename list::template element<Index> > ())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (Predicate{}.template operator() < Index, typename list::template element<Index> > ())
							{
								next_index_array[next_size] = Index;
								++next_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Data.size>{});
#endif

					return ViewData{ next_index_array, NextOffset, next_size };
				} > {};
			}

			template<typename Predicate>
				requires (private_data::data.size == 0) || requires
			{
				bool{ Predicate{}.template operator()
					< private_data::data[0], typename list::template element<private_data::data[0]> > () };
			}
			static constexpr auto remove_with_index(Predicate&&)
			{
				return type_list_view < CurrentView, [] <auto Data> {

					constexpr std::size_t NextOffset{ 0 };

					std::array<index_type, Data.size> next_index_array{};
					std::size_t next_size{ 0 };

#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
					//C++26
					template for (constexpr auto Index : Data)
					{
						if constexpr (!Predicate{}.template operator() < Index, typename list::template element<Index> > ())
						{
							next_index_array[next_size] = Index;
							++next_size;
						}
					}
#else
					//C++20
					[&] <std::size_t... I> (std::index_sequence<I...>) {
						(..., [&] <auto Index> {
							if constexpr (!Predicate{}.template operator() < Index, typename list::template element<Index> > ())
							{
								next_index_array[next_size] = Index;
								++next_size;
							}
						}.template operator() < Data[I] > ());
					}(std::make_index_sequence<Data.size>{});
#endif

					return ViewData{ next_index_array, NextOffset, next_size };
				} > {};
			}


			template<typename EquivalenceRelation>
				requires (private_data::data.size <= 1) || requires
			{
				bool{ EquivalenceRelation{}.template operator()
					<
						typename list::template element<private_data::data[0]>,
						typename list::template element<private_data::data[1]>
					> () };
			}
			static constexpr auto unique(EquivalenceRelation&&)
			{




			}

			template<typename PartialOrder>
				requires (private_data::data.size <= 1) || requires
			{
				bool{ PartialOrder{}.template operator()
					<
						typename list::template element<private_data::data[0]>,
						typename list::template element<private_data::data[1]>
					> () };
			}
			static constexpr auto unique_with_partial_order(PartialOrder&&)
			{




			}

			template<typename PartialOrder>
				requires (private_data::data.size <= 1) || requires
			{
				bool{ PartialOrder{}.template operator()
					<
						typename list::template element<private_data::data[0]>,
						typename list::template element<private_data::data[1]>
					> () };
			}
			static constexpr auto sort(PartialOrder&&)
			{





			}


			template<template<typename...> typename TypeWrapper = type_list>
				requires requires { private_data::data; } //用于创建非特化路径的 bake() 来提示 IDE 返回类型
			static constexpr auto bake()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename list::template element<private_data::data[I]>...
					>{};
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
					>{};
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
					>{};
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
			static constexpr auto unique(EquivalenceRelation&&)
			{
				static_assert(dependent_false_v<EquivalenceRelation>,
					"[logicwise] Error: Incompatible equivalence relation signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename PartialOrder>
			static constexpr auto unique_with_partial_order(PartialOrder&&)
			{
				static_assert(dependent_false_v<PartialOrder>,
					"[logicwise] Error: Incompatible partial order signature!\n"
					"Expected non-capturing lambda: [] <typename Type1, typename Type2>() -> bool { ... }");

				return CurrentView{};
			}

			template<typename PartialOrder>
			static constexpr auto sort(PartialOrder&&)
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

		template<typename PreviousView, auto ViewAdaptor = identity>
		struct value_list_view
		{
			using list = typename PreviousView::list;

			struct private_data
			{
				static constexpr auto data{ ViewAdaptor.template operator() < PreviousView::private_data::get_data() > () };
				static constexpr auto get_data() noexcept { return data; }
			};

			using CurrentView = value_list_view;




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

		template<VectorLike ContainerType>
		[[nodiscard]] static constexpr auto in(ContainerType&& container)
		{
			return initial_container_view<ContainerType>{ std::forward<ContainerType>(container) };
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
				"[logicwise] Error: Incompatible container!\n"
				"Expected: VectorLike container.");

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


		template<typename ContainerType>
		struct initial_container_view
		{
			using ContainerTrait = vector_like_container_trait<ContainerType>;

			using RawInstanceType = typename ContainerTrait::raw_instance_type;
			using StoredInstanceType = typename ContainerTrait::stored_instance_type;
			using StoredContainerType = typename ContainerTrait::stored_container_type;
			using ExpectedContainerType = typename ContainerTrait::expected_container_type;

			const StoredContainerType container;

			initial_container_view() = delete; //禁用默认构造函数

			explicit constexpr initial_container_view(ExpectedContainerType container) noexcept
				: container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}
		

		};

	};

}

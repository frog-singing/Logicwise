// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/mode/view/view_adaptor.h>
#include "agent.h"
#include "error_reporter.h"
#include <utility> //用于 std::forward, std::index_sequence, std::make_index_sequence
#include <type_traits> //用于 std::remove_reference_t
#include <cstddef> //用于 std::size_t
#include <variant> //用于 std::monostate
#include <array> //用于 std::array


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	template<typename List>
	struct value_list_view_protocol
	{
		using material_agent			= value_list_agent<List>;
		using dependent_view_adaptor	= view_adaptor<material_agent>;
		using error_reporter			= value_list_error_reporter;

		template<auto Data>
		struct materializer
		{
			template<template<auto...> typename ValueWrapper = value_list>
			static constexpr auto bake()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return ValueWrapper<
						List::template element<Data[I]>...
					>{};
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
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename decltype(std::remove_reference_t<Mapping>{}.template operator()
							< List::template element<Data[I]> > ())::type...
					>{};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<auto> typename Mapping, template<typename...> typename TypeWrapper = type_list>
			static constexpr auto forge_into_type()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper <
						Mapping< List::template element<Data[I]> >...
					>{};
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
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return ValueWrapper <
						std::remove_reference_t<Mapping>{}.template operator()
						< List::template element<Data[I]> > ()...
					>{};
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
					return [&] <std::size_t... I> (std::index_sequence<I...>) {
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

}

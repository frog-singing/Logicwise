// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/mode/view/adaptor/template_view_adaptor.h>
#include "agent.h"
#include "error_reporter.h"
#include <utility> //用于 std::index_sequence, std::make_index_sequence, std::forward
#include <type_traits> //用于 std::remove_reference_t
#include <cstddef> //用于 std::size_t
#include <variant> //用于 std::monostate
#include <array> //用于 std::array


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 type_list 的实例
	template<typename List>
	struct type_list_view_protocol
	{
		using material_agent	= type_list_agent<List>;
		using view_adaptor		= template_view_adaptor;
		using error_reporter	= type_list_error_reporter;

		template<auto Data>
		struct materializer
		{
			template<template<typename...> typename TypeWrapper = type_list>
			[[nodiscard]] static constexpr auto bake()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename List::template element<Data[I]>...
					>{};
				}(std::make_index_sequence<Data.size>{});
			}

			//只能用于 decltype
			//for decltype only
			template<template<typename...> typename TypeWrapper = type_list>
			[[nodiscard]] static constexpr auto bake_for_decltype()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) ->
					TypeWrapper<
						typename List::template element<Data[I]>...
					>
				{ /*不安全 unsafe*/ }(std::make_index_sequence<Data.size>{});
			}

			template<template<typename...> typename TypeWrapper = type_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				typename decltype(std::remove_reference_t<Mapping>{}.template operator()
					< typename List::template element<Data[0]> > ())::type;
			}
			[[nodiscard]] static constexpr auto forge_into_type(Mapping&&)
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename decltype(std::remove_reference_t<Mapping>{}.template operator()
							< typename List::template element<Data[I]> > ())::type...
					>{};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<typename> typename Mapping, template<typename...> typename TypeWrapper = type_list>
			[[nodiscard]] static constexpr auto forge_into_type()
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper <
						Mapping< typename List::template element<Data[I]> >...
					>{};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<auto...> typename ValueWrapper = value_list, typename Mapping>
				requires (Data.size == 0) || requires
			{
				std::remove_reference_t<Mapping>{}.template operator()
					< typename List::template element<Data[0]> > ();
			}
			[[nodiscard]] static constexpr auto forge_into_value(Mapping&&)
			{
				return [] <std::size_t... I> (std::index_sequence<I...>) {
					return ValueWrapper <
						std::remove_reference_t<Mapping>{}.template operator()
						< typename List::template element<Data[I]> > ()...
					>{};
				}(std::make_index_sequence<Data.size>{});
			}

			template<typename MappingType>
				requires (Data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< typename List::template element<Data[0]> > ();
			}
			[[nodiscard]] static constexpr auto forge_into_instance(MappingType&& mapping)
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
								< typename List::template element<Data[I]> > ()...
						};
					}(std::make_index_sequence<Data.size>{});
				}
			}

			template<typename ContainerType, typename MappingType>
				requires (Data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< typename List::template element<Data[0]> > ();
			}
			[[nodiscard]] static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				return [&] <std::size_t... I> (std::index_sequence<I...>) {
					return ContainerType{
						mapping.template operator()
							< typename List::template element<Data[I]> > ()...
					};
				}(std::make_index_sequence<Data.size>{});
			}

			template<template<typename...> typename TypeWrapper, typename MappingType>
				requires (Data.size == 0) || requires(MappingType && mapping)
			{
				std::forward<MappingType>(mapping).template operator()
					< typename List::template element<Data[0]> > ();
			}
			[[nodiscard]] static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				return [&] <std::size_t... I> (std::index_sequence<I...>) {
					return TypeWrapper<
						typename List::template element<Data[I]>...
					>
					{
						mapping.template operator()
							< typename List::template element<Data[I]> > ()...
					};
				}(std::make_index_sequence<Data.size>{});
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

			template<typename ContainerType, typename MappingType>
			[[nodiscard]] static constexpr auto forge_into_instance(MappingType&&)
			{
				static_assert(dependent_false_v<MappingType>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected: [] <typename Type>() -> auto { ... }");

				return ContainerType{};
			}

			template<template<typename...> typename TypeWrapper, typename MappingType>
			[[nodiscard]] static constexpr auto forge_into_instance(MappingType&& mapping)
			{
				static_assert(dependent_false_v<MappingType>,
					"[logicwise] Error: Incompatible mapping signature!\n"
					"Expected: [] <typename Type>() -> auto { ... }");

				return TypeWrapper<>{};
			}

		};

		template<typename ViewWorkshop>
		struct extended_core
		{
			using index_type = typename material_agent::index_type;

			template<template<typename> typename Predicate, index_type ProbeIndex>
			using trait_predicate_solver = typename material_agent::template trait_predicate_solver<Predicate, ProbeIndex>;

			template<template<typename, typename> typename HomogeneousRelation, index_type ProbeIndex1, index_type ProbeIndex2>
			using trait_homogeneous_relation_solver =
				typename material_agent::template trait_homogeneous_relation_solver<HomogeneousRelation, ProbeIndex1, ProbeIndex2>;

			//--------------------------------------------------------------------------------

			using basic_core	= typename ViewWorkshop::basic_core;
			using extent_trait	= typename ViewWorkshop::extent_trait;

			template<typename Adaptor>
			using new_view = typename ViewWorkshop::template new_view<Adaptor>;

			//--------------------------------------------------------------------------------

			static constexpr auto probe_index = extent_trait::probe_index;

			//================================================================================

			struct core : basic_core
			{
				using basic_core::take_while;
				using basic_core::take_until;
				using basic_core::drop_while;
				using basic_core::drop_until;
				using basic_core::take_last_while;
				using basic_core::take_last_until;
				using basic_core::drop_last_while;
				using basic_core::drop_last_until;

				using basic_core::select;
				using basic_core::remove;

				using basic_core::unique_first;
				using basic_core::unique_last;
				using basic_core::unique_adjacent_first;
				using basic_core::unique_adjacent_last;

				using basic_core::sort;

				using basic_core::sort_with_partial_weak_order;
				using basic_core::unique_first_with_partial_weak_order;
				using basic_core::unique_last_with_partial_weak_order;

				using basic_core::sort_topologically;

				using basic_core::sort_topologically_with_partial_partial_order;

				//================================================================================

				template<template<typename> typename Predicate>
				static constexpr auto take_while()
				{
					return new_view< typename view_adaptor::template take_while<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto take_until()
				{
					return new_view< typename view_adaptor::template take_until<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto drop_while()
				{
					return new_view< typename view_adaptor::template drop_while<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto drop_until()
				{
					return new_view< typename view_adaptor::template drop_until<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto take_last_while()
				{
					return new_view< typename view_adaptor::template take_last_while<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto take_last_until()
				{
					return new_view< typename view_adaptor::template take_last_until<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto drop_last_while()
				{
					return new_view< typename view_adaptor::template drop_last_while<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto drop_last_until()
				{
					return new_view< typename view_adaptor::template drop_last_until<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename> typename Predicate>
				static constexpr auto select()
				{
					return new_view< typename view_adaptor::template select<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				template<template<typename> typename Predicate>
				static constexpr auto remove()
				{
					return new_view< typename view_adaptor::template remove<
						trait_predicate_solver<Predicate, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename, typename> typename EquivalenceRelation>
				static constexpr auto unique_first()
				{
					return new_view< typename view_adaptor::template unique_first<
						trait_homogeneous_relation_solver<EquivalenceRelation, probe_index, probe_index>
					> >{};
				}

				template<template<typename, typename> typename EquivalenceRelation>
				static constexpr auto unique_last()
				{
					return new_view< typename view_adaptor::template unique_last<
						trait_homogeneous_relation_solver<EquivalenceRelation, probe_index, probe_index>
					> >{};
				}

				template<template<typename, typename> typename EquivalenceRelation>
				static constexpr auto unique_adjacent_first()
				{
					return new_view< typename view_adaptor::template unique_adjacent_first<
						trait_homogeneous_relation_solver<EquivalenceRelation, probe_index, probe_index>
					> >{};
				}

				template<template<typename, typename> typename EquivalenceRelation>
				static constexpr auto unique_adjacent_last()
				{
					return new_view< typename view_adaptor::template unique_adjacent_last<
						trait_homogeneous_relation_solver<EquivalenceRelation, probe_index, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename, typename> typename StrictWeakOrder>
				static constexpr auto sort()
				{
					return new_view< typename view_adaptor::template sort<
						trait_homogeneous_relation_solver<StrictWeakOrder, probe_index, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename, typename> typename PartialWeakOrder>
				static constexpr auto sort_with_partial_weak_order()
				{
					return new_view< typename view_adaptor::template sort_with_partial_weak_order<
						trait_homogeneous_relation_solver<PartialWeakOrder, probe_index, probe_index>
					> >{};
				}

				template<template<typename, typename> typename PartialWeakOrder>
				static constexpr auto unique_first_with_partial_weak_order()
				{
					return new_view< typename view_adaptor::template unique_first_with_partial_weak_order<
						trait_homogeneous_relation_solver<PartialWeakOrder, probe_index, probe_index>
					> >{};
				}

				template<template<typename, typename> typename PartialWeakOrder>
				static constexpr auto unique_last_with_partial_weak_order()
				{
					return new_view< typename view_adaptor::template unique_last_with_partial_weak_order<
						trait_homogeneous_relation_solver<PartialWeakOrder, probe_index, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename, typename> typename AcyclicRelation>
				static constexpr auto sort_topologically()
				{
					return new_view< typename view_adaptor::template sort_topologically<
						trait_homogeneous_relation_solver<AcyclicRelation, probe_index, probe_index>
					> >{};
				}

				//--------------------------------------------------------------------------------

				template<template<typename, typename> typename PartialPartialOrder>
				static constexpr auto sort_topologically_with_partial_partial_order()
				{
					return new_view< typename view_adaptor::template sort_topologically_with_partial_partial_order<
						trait_homogeneous_relation_solver<PartialPartialOrder, probe_index, probe_index>
					> >{};
				}

			};

		};

	};

}

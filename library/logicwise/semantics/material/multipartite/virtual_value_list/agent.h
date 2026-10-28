// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/index/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/computation/template_unary_computation_operator.h>
#include <logicwise/semantics/computation/template_binary_computation_operator.h>

//用于 elementwise_viewing<view, element>
#include <logicwise/mode/view/elementwise.h>
#include <logicwise/mode/type.h>
#include <logicwise/arrangement/elementwise/element.h>

#include <cstddef> //用于 std::size_t
#include <type_traits> //用于 std::remove_reference_t
#include <utility> //用于 std::index_sequence, std::make_index_sequence, std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	template<typename... List>
	struct virtual_multipartite_value_list_agent
	{
		template<template<std::size_t Arity> typename ElementResolver>
		struct with_element_resolver
		{
			using role_tag				= material_agent_role_tag;
			using material_domain_tag	= template_material_domain_tag;
			using organization_form_tag	= virtual_organization_form_tag;
			using material_feeding_tag	= packable_material_feeding_tag;

			//--------------------------------------------------------------------------------

			static constexpr std::size_t arity{ sizeof...(List) };

			using element_resolver = ElementResolver<arity>;
			using index_type = typename element_resolver::index_type;

			using extent_type = Extent<arity>;
			static constexpr extent_type extent{ List::size... };

			template<MultipartiteArrangement Arrangement,
				typename IndexTraverserType = typename Arrangement::light_index_traverser>
			using extent_trait = arrangement_extent_trait<Arrangement, extent, IndexTraverserType>;

			//基本计算原语 basic computation primitive================================================================================

			//应用
			template<typename IndexSequence> struct apply_at_impl;
			template<std::size_t... I> struct apply_at_impl<std::index_sequence<I...>>
			{
				template<template<auto...> typename Wrapper, index_type Index>
				using apply_at = Wrapper<
					element_resolver::template element<List, Index, I>...
				>;
			};

			template<template<auto...> typename Wrapper, index_type Index>
			using apply_at = typename apply_at_impl<std::make_index_sequence<arity>>
				::template apply_at<Wrapper, Index>;

			//检索
			template<index_type Index>
			static constexpr auto lookup()
			{
				return elementwise_viewing<view, element>::in< apply_at<value_list, Index> >();
			}

			//私有计算算子 private computation operator================================================================================

			//谓词 predicate--------------------------------------------------------------------------------



		};

	};

}

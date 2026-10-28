// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/index/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/semantics/tag.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/computation/template_unary_computation_operator.h>
#include <logicwise/semantics/computation/template_binary_computation_operator.h>
#include <type_traits> //用于 std::remove_reference_t
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 value_list 的实例
	template<typename List>
	struct value_list_agent
	{
		using role_tag				= material_agent_role_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
		using material_feeding_tag	= non_packable_material_feeding_tag;

		//--------------------------------------------------------------------------------

		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		using extent_type = Extent1D;
		static constexpr extent_type extent{ List::size };

		template<ElementwiseArrangement Arrangement,
			typename IndexTraverserType = typename Arrangement::light_index_traverser>
		using extent_trait = arrangement_extent_trait<Arrangement, extent, IndexTraverserType>;

		//基本计算原语 basic computation primitive================================================================================

		//应用
		template<template<auto> typename Wrapper, index_type Index>
		using apply_at = Wrapper<
			List::template element<Index>
		>;

		//检索
		template<index_type Index>
		static constexpr auto lookup()
		{
			return List::template element<Index>;
		}

		//私有计算算子 private computation operator================================================================================

		//谓词 predicate--------------------------------------------------------------------------------

		template<template<auto> typename Predicate, index_type ProbeIndex>
		struct trait_predicate_solver
		{
			static_assert(requires{ typename apply_at<Predicate, ProbeIndex>; },
				"[logicwise] Trait predicate instantiation failed at probe index.");

			using TraitCertificate = apply_at<Predicate, ProbeIndex>;

			static constexpr auto SolverProxy{ detail::trait_predicate_solver<TraitCertificate> };

			template<index_type Index>
			static constexpr bool solve()
			{
				return SolverProxy.template operator() < apply_at<Predicate, Index> > ();
			}

			template<index_type Index>
			constexpr bool operator()()
			{
				return solve<Index>();
			}
		};

		//组合子 combinator--------------------------------------------------------------------------------

		template<template<typename Accumulator, auto Value> typename Combinator>
		struct alias_to_type_combinator_folder
		{
			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Accumulator,
				List::template element<Index>
			>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//--------------------------------------------------------------------------------

		template<template<auto Index, typename Accumulator, auto Value> typename Combinator>
		struct alias_to_type_combinator_with_index_folder
		{
			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Index,
				Accumulator,
				List::template element<Index>
			>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//同质关系 homogeneous relation--------------------------------------------------------------------------------

		template<template<auto, auto> typename HomogeneousRelation, index_type ProbeIndex1, index_type ProbeIndex2>
		struct trait_homogeneous_relation_solver
		{
			template<index_type Index1, index_type Index2>
			using apply_at = HomogeneousRelation<
				List::template element<Index1>,
				List::template element<Index2>
			>;

			static_assert(requires{ typename apply_at<ProbeIndex1, ProbeIndex2>; },
				"[logicwise] Trait homogeneous relation instantiation failed at probe index pair.");

			using TraitCertificate = apply_at<ProbeIndex1, ProbeIndex2>;

			static constexpr auto SolverProxy{ detail::trait_predicate_solver<TraitCertificate> };

			template<index_type Index1, index_type Index2>
			static constexpr bool solve()
			{
				return SolverProxy.template operator() < apply_at<Index1, Index2> > ();
			}

			template<index_type Index1, index_type Index2>
			constexpr bool operator()()
			{
				return solve<Index1, Index2>();
			}
		};


		//一元计算原语
		struct unary_computation_primitive
		{
			//逐元素
			struct elementwise
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						List::template element<Index>
					> ();
				}

				template<typename Invocable, index_type Index>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<Index>(std::remove_reference_t<Invocable>{});
				}

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
				{
					return invoke_at_impl<Index>(std::forward<InvocableType>(invocable));
				}

				//组合 combine--------------------------------------------------------------------------------

				template<typename Combinator, typename Accumulator, index_type Index>
				using combine_type_at = typename decltype(
					std::remove_reference_t<Combinator>{}.template operator() <
						Accumulator,
						List::template element<Index>
					> ()
				)::type;

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return std::remove_reference_t<Combinator>{}.template operator() <
						Accumulator,
						List::template element<Index>
					> ();
				}

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return std::forward<CombinatorType>(combinator).template operator() <
						List::template element<Index>
					> (std::forward<AccumulatorType>(accumulator));
				}

			};

			//带索引逐元素
			struct elementwise_with_index
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						Index,
						List::template element<Index>
					> ();
				}

				template<typename Invocable, index_type Index>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<Index>(std::remove_reference_t<Invocable>{});
				}

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
				{
					return invoke_at_impl<Index>(std::forward<InvocableType>(invocable));
				}

				//组合 combine--------------------------------------------------------------------------------

				template<typename Combinator, typename Accumulator, index_type Index>
				using combine_type_at = typename decltype(
					std::remove_reference_t<Combinator>{}.template operator() <
						Index,
						Accumulator,
						List::template element<Index>
					> ()
				)::type;

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return std::remove_reference_t<Combinator>{}.template operator() <
						Index,
						Accumulator,
						List::template element<Index>
					> ();
				}

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return std::forward<CombinatorType>(combinator).template operator() <
						Index,
						List::template element<Index>
					> (std::forward<AccumulatorType>(accumulator));
				}

			};

		};


		//二元计算原语
		struct binary_computation_primitive
		{
			//逐元素
			struct elementwise
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index1, index_type Index2, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						List::template element<Index1>,
						List::template element<Index2>
					> ();
				}

				template<typename Invocable, index_type Index1, index_type Index2>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<Index1, Index2>(std::remove_reference_t<Invocable>{});
				}

			};

		};


		//计算算子
		struct computation_operator
		{
			struct elementwise
				: template_unary_computation_operator
					<typename unary_computation_primitive::elementwise>
				, template_binary_computation_operator
					<typename binary_computation_primitive::elementwise>
			{};

			struct elementwise_with_index
				: template_unary_computation_operator
					<typename unary_computation_primitive::elementwise_with_index>
			{};

		};

	};

}

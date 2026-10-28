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

	//确保 List 是 type_list 的实例
	template<typename List, std::size_t Arity>
	struct multipletwise_type_list_agent
	{
		using role_tag				= material_agent_role_tag;
		using material_domain_tag	= template_material_domain_tag;
		using organization_form_tag	= concrete_organization_form_tag;
		using material_feeding_tag	= packable_material_feeding_tag;

		//--------------------------------------------------------------------------------

		static constexpr std::size_t arity{ Arity };

		using index_trait = IndexTrait<arity>;
		using index_type = typename index_trait::index_type;

		using extent_type = Extent1D;
		static constexpr extent_type extent{ List::size };

		template<MultipletwiseArrangement Arrangement,
			typename IndexTraverserType = typename Arrangement::light_index_traverser>
		using extent_trait = arrangement_extent_trait<Arrangement, extent, IndexTraverserType>;

		//基本计算原语 basic computation primitive================================================================================

		//应用
		template<typename IndexSequence> struct apply_at_impl;
		template<std::size_t... I> struct apply_at_impl<std::index_sequence<I...>>
		{
			template<template<auto...> typename Wrapper, index_type Index>
			using apply_at = Wrapper<
				List::template element<Index[I]>...
			>;
		};

		template<template<auto...> typename Wrapper, index_type Index>
		using apply_at = typename apply_at_impl<std::make_index_sequence<arity>>
			::template apply_at<Wrapper, Index>;

		//检索
		template<index_type Index>
		static constexpr auto lookup()
		{
			return elementwise_viewing<view, element>::in< apply_at<type_list, Index> >();
		}

		//私有计算算子 private computation operator================================================================================

		//谓词 predicate--------------------------------------------------------------------------------

		template<template<auto...> typename Predicate, index_type ProbeIndex>
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

		template<template<typename Accumulator, auto... Value> typename Combinator>
		struct alias_to_type_combinator_folder
		{
			template<typename IndexSequence> struct fold_impl;
			template<std::size_t... I> struct fold_impl<std::index_sequence<I...>>
			{
				template<typename Accumulator, index_type Index>
				using fold = Combinator<
					Accumulator,
					List::template element<Index[I]>...
				>;
			};

			template<typename Accumulator, index_type Index>
			using fold = typename fold_impl<std::make_index_sequence<arity>>
				::template fold<Accumulator, Index>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//--------------------------------------------------------------------------------

		template<template<auto Index, typename Accumulator, auto... Value> typename Combinator>
		struct alias_to_type_combinator_with_index_folder
		{
			template<typename IndexSequence> struct fold_impl;
			template<std::size_t... I> struct fold_impl<std::index_sequence<I...>>
			{
				template<typename Accumulator, index_type Index>
				using fold = Combinator<
					Index,
					Accumulator,
					List::template element<Index[I]>...
				>;
			};

			template<typename Accumulator, index_type Index>
			using fold = typename fold_impl<std::make_index_sequence<arity>>
				::template fold<Accumulator, Index>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//--------------------------------------------------------------------------------

		template<template<typename Accumulator, auto View> typename Combinator>
		struct alias_rangewise_to_type_combinator_folder
		{
			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Accumulator,
				lookup<Index>()
			>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//--------------------------------------------------------------------------------

		template<template<auto Index, typename Accumulator, auto View> typename Combinator>
		struct alias_rangewise_to_type_combinator_with_index_folder
		{
			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Index,
				Accumulator,
				lookup<Index>()
			>;

			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename fold<Accumulator, Index>;
			};
		};

		//同质关系 homogeneous relation--------------------------------------------------------------------------------

		template<template<auto...> typename HomogeneousRelation, index_type ProbeIndex1, index_type ProbeIndex2>
		struct trait_homogeneous_relation_solver
		{
			template<typename IndexSequence> struct apply_at_impl;
			template<std::size_t... I> struct apply_at_impl<std::index_sequence<I...>>
			{
				template<index_type Index1, index_type Index2>
				using apply_at = HomogeneousRelation<
					List::template element<Index1[I]>...,
					List::template element<Index2[I]>...
				>;
			};

			template<index_type Index1, index_type Index2>
			using apply_at = typename apply_at_impl<std::make_index_sequence<arity>>
				::template apply_at<Index1, Index2>;

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

				template<typename IndexSequence> struct invoke_at_impl;
				template<std::size_t... I> struct invoke_at_impl<std::index_sequence<I...>>
				{
					template<index_type Index, typename InvocableType>
					static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
					{
						return std::forward<InvocableType>(invocable).template operator() <
							List::template element<Index[I]>...
						> ();
					}
				};

				template<typename Invocable, index_type Index>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<std::make_index_sequence<arity>>
						::template invoke_at<Index>(std::remove_reference_t<Invocable>{});
				}

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
				{
					return invoke_at_impl<std::make_index_sequence<arity>>
						::template invoke_at<Index>(std::forward<InvocableType>(invocable));
				}

				//组合 combine--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_type_at_impl;
				template<std::size_t... I> struct combine_type_at_impl<std::index_sequence<I...>>
				{
					template<typename Combinator, typename Accumulator, index_type Index>
					using combine_type_at = typename decltype(
						std::remove_reference_t<Combinator>{}.template operator() <
							Accumulator,
							List::template element<Index[I]>...
						> ()
					)::type;
				};

				template<typename Combinator, typename Accumulator, index_type Index>
				using combine_type_at = typename combine_type_at_impl<std::make_index_sequence<arity>>
					::template combine_type_at<Combinator, Accumulator, Index>;

				//--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_value_at_impl;
				template<std::size_t... I> struct combine_value_at_impl<std::index_sequence<I...>>
				{
					template<typename Combinator, auto Accumulator, index_type Index>
					static constexpr decltype(auto) combine_value_at()
					{
						return std::remove_reference_t<Combinator>{}.template operator() <
							Accumulator,
							List::template element<Index[I]>...
						> ();
					}
				};

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return combine_value_at_impl<std::make_index_sequence<arity>>
						::template combine_value_at<Combinator, Accumulator, Index>();
				}

				//--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_instance_at_impl;
				template<std::size_t... I> struct combine_instance_at_impl<std::index_sequence<I...>>
				{
					template<index_type Index, typename CombinatorType, typename AccumulatorType>
					static constexpr decltype(auto) combine_instance_at(
						CombinatorType&& combinator, AccumulatorType&& accumulator)
					{
						return std::forward<CombinatorType>(combinator).template operator() <
							List::template element<Index[I]>...
						> (std::forward<AccumulatorType>(accumulator));
					}
				};

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return combine_instance_at_impl<std::make_index_sequence<arity>>
						::template combine_instance_at<Index>(
							std::forward<CombinatorType>(combinator),
							std::forward<AccumulatorType>(accumulator)
						);
				}

			};

			//带索引逐元素
			struct elementwise_with_index
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<typename IndexSequence> struct invoke_at_impl;
				template<std::size_t... I> struct invoke_at_impl<std::index_sequence<I...>>
				{
					template<index_type Index, typename InvocableType>
					static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
					{
						return std::forward<InvocableType>(invocable).template operator() <
							Index,
							List::template element<Index[I]>...
						> ();
					}
				};

				template<typename Invocable, index_type Index>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<std::make_index_sequence<arity>>
						::template invoke_at<Index>(std::remove_reference_t<Invocable>{});
				}

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
				{
					return invoke_at_impl<std::make_index_sequence<arity>>
						::template invoke_at<Index>(std::forward<InvocableType>(invocable));
				}

				//组合 combine--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_type_at_impl;
				template<std::size_t... I> struct combine_type_at_impl<std::index_sequence<I...>>
				{
					template<typename Combinator, typename Accumulator, index_type Index>
					using combine_type_at = typename decltype(
						std::remove_reference_t<Combinator>{}.template operator() <
							Index,
							Accumulator,
							List::template element<Index[I]>...
						> ()
					)::type;
				};

				template<typename Combinator, typename Accumulator, index_type Index>
				using combine_type_at = typename combine_type_at_impl<std::make_index_sequence<arity>>
					::template combine_type_at<Combinator, Accumulator, Index>;

				//--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_value_at_impl;
				template<std::size_t... I> struct combine_value_at_impl<std::index_sequence<I...>>
				{
					template<typename Combinator, auto Accumulator, index_type Index>
					static constexpr decltype(auto) combine_value_at()
					{
						return std::remove_reference_t<Combinator>{}.template operator() <
							Index,
							Accumulator,
							List::template element<Index[I]>...
						> ();
					}
				};

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return combine_value_at_impl<std::make_index_sequence<arity>>
						::template combine_value_at<Combinator, Accumulator, Index>();
				}

				//--------------------------------------------------------------------------------

				template<typename IndexSequence> struct combine_instance_at_impl;
				template<std::size_t... I> struct combine_instance_at_impl<std::index_sequence<I...>>
				{
					template<index_type Index, typename CombinatorType, typename AccumulatorType>
					static constexpr decltype(auto) combine_instance_at(
						CombinatorType&& combinator, AccumulatorType&& accumulator)
					{
						return std::forward<CombinatorType>(combinator).template operator() <
							Index,
							List::template element<Index[I]>...
						> (std::forward<AccumulatorType>(accumulator));
					}
				};

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return combine_instance_at_impl<std::make_index_sequence<arity>>
						::template combine_instance_at<Index>(
							std::forward<CombinatorType>(combinator),
							std::forward<AccumulatorType>(accumulator)
						);
				}

			};

			//按范围
			struct rangewise
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						lookup<Index>()
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
						lookup<Index>()
					> ()
				)::type;

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return std::remove_reference_t<Combinator>{}.template operator() <
						Accumulator,
						lookup<Index>()
					> ();
				}

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return std::forward<CombinatorType>(combinator).template operator() <
						lookup<Index>()
					> (std::forward<AccumulatorType>(accumulator));
				}

			};

			//带索引按范围
			struct rangewise_with_index
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						Index,
						lookup<Index>()
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
						lookup<Index>()
					> ()
				)::type;

				template<typename Combinator, auto Accumulator, index_type Index>
				static constexpr decltype(auto) combine_value_at()
				{
					return std::remove_reference_t<Combinator>{}.template operator() <
						Index,
						Accumulator,
						lookup<Index>()
					> ();
				}

				template<index_type Index, typename CombinatorType, typename AccumulatorType>
				static constexpr decltype(auto) combine_instance_at(
					CombinatorType&& combinator, AccumulatorType&& accumulator)
				{
					return std::forward<CombinatorType>(combinator).template operator() <
						Index,
						lookup<Index>()
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

				template<typename IndexSequence> struct invoke_at_impl;
				template<std::size_t... I> struct invoke_at_impl<std::index_sequence<I...>>
				{
					template<index_type Index1, index_type Index2, typename InvocableType>
					static constexpr decltype(auto) invoke_at(InvocableType&& invocable)
					{
						return std::forward<InvocableType>(invocable).template operator() <
							List::template element<Index1[I]>...,
							List::template element<Index2[I]>...
						> ();
					}
				};

				template<typename Invocable, index_type Index1, index_type Index2>
				static constexpr decltype(auto) invoke_at()
				{
					return invoke_at_impl<std::make_index_sequence<arity>>
						::template invoke_at<Index1, Index2>(std::remove_reference_t<Invocable>{});
				}

			};

			//按范围
			struct rangewise
			{
				//调用 invoke--------------------------------------------------------------------------------

				template<index_type Index1, index_type Index2, typename InvocableType>
				static constexpr decltype(auto) invoke_at_impl(InvocableType&& invocable)
				{
					return std::forward<InvocableType>(invocable).template operator() <
						lookup<Index1>(),
						lookup<Index2>()
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

			struct rangewise
				: template_unary_computation_operator
					<typename unary_computation_primitive::rangewise>
				, template_binary_computation_operator
					<typename binary_computation_primitive::rangewise>
			{};

			struct rangewise_with_index
				: template_unary_computation_operator
					<typename unary_computation_primitive::rangewise_with_index>
			{};

		};

	};

}

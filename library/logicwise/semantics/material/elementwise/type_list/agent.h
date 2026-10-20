// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/element.h>
#include <logicwise/index/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/semantics/trait_predicate.h>
#include <type_traits> //用于 std::remove_reference_t
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	//确保 List 是 type_list 的实例
	template<typename List>
	struct type_list_agent
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		using extent_type = Extent1D;
		static constexpr extent_type extent{ List::size };

		template<ElementwiseArrangement Arrangement,
			typename IndexTraverserType = typename Arrangement::light_index_traverser>
		using extent_trait = arrangement_extent_trait<Arrangement, extent, IndexTraverserType>;

		//检索 lookup================================================================================

		template<index_type Index>
		static constexpr auto lookup()
		{
			return type_element< typename List::template element<Index> >{};
		}

		//谓词 predicate================================================================================

		template<typename Predicate>
		struct predicate_solver
		{
			template<index_type Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					typename List::template element<Index>
				> () };
			};

			template<index_type Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					typename List::template element<Index>
				> ();
			}
		};

		//--------------------------------------------------------------------------------

		template<typename PredicateType, index_type Index>
		static constexpr bool solvable_predicate_type = requires(PredicateType&& predicate)
		{
			bool{ std::forward<PredicateType>(predicate).template operator() <
				typename List::template element<Index>
			> () };
		};

		template<index_type Index, typename PredicateType>
		static constexpr bool solve_predicate(PredicateType&& predicate)
		{
			return std::forward<PredicateType>(predicate).template operator() <
				typename List::template element<Index>
			> ();
		}

		//--------------------------------------------------------------------------------

		//用于 extended core

		template<template<typename> typename Predicate, index_type Index>
		static constexpr bool solvable_trait_predicate = requires
		{
			typename Predicate<
				typename List::template element<Index>
			>;
		};

		template<template<typename> typename Predicate, index_type ProbeIndex>
		struct trait_predicate_solver
		{
			using TraitCertificate = Predicate<
				typename List::template element<ProbeIndex>
			>;

			static constexpr auto SolverProxy{ detail::trait_predicate_solver<TraitCertificate> };

			template<index_type Index>
			static constexpr bool solve()
			{
				return SolverProxy.template operator() < Predicate<
					typename List::template element<Index>
				> > ();
			}
		};

		//带索引谓词 predicate with index================================================================================

		template<typename Predicate>
		struct predicate_with_index_solver
		{
			template<index_type Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					typename List::template element<Index>
				> () };
			};

			template<index_type Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					typename List::template element<Index>
				> ();
			}
		};

		//--------------------------------------------------------------------------------

		template<typename PredicateType, index_type Index>
		static constexpr bool solvable_predicate_with_index_type = requires(PredicateType&& predicate)
		{
			bool{ std::forward<PredicateType>(predicate).template operator() <
				Index,
				typename List::template element<Index>
			> () };
		};

		template<index_type Index, typename PredicateType>
		static constexpr bool solve_predicate_with_index(PredicateType&& predicate)
		{
			return std::forward<PredicateType>(predicate).template operator() <
				Index,
				typename List::template element<Index>
			> ();
		}

		//同质关系 homogeneous relation================================================================================

		template<typename HomogeneousRelation>
		struct homogeneous_relation_solver
		{
			template<index_type Index1, index_type Index2>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					typename List::template element<Index1>,
					typename List::template element<Index2>
				> () };
			};

			template<index_type Index1, index_type Index2>
			static constexpr bool solve()
			{
				return std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					typename List::template element<Index1>,
					typename List::template element<Index2>
				> ();
			}
		};

		//--------------------------------------------------------------------------------

		/*
		template<typename HomogeneousRelationType, index_type Index1, index_type Index2>
		static constexpr bool solvable_homogeneous_relation_type = requires(HomogeneousRelationType&& homogeneous_relation)
		{
			bool{ std::forward<HomogeneousRelationType>(homogeneous_relation).template operator() <
				typename List::template element<Index1>,
				typename List::template element<Index2>
			> () };
		};

		template<index_type Index1, index_type Index2, typename HomogeneousRelationType>
		static constexpr bool solve_homogeneous_relation(HomogeneousRelationType&& homogeneous_relation)
		{
			return std::forward<HomogeneousRelationType>(homogeneous_relation).template operator() <
				typename List::template element<Index1>,
				typename List::template element<Index2>
			> ();
		}
		*/

		//--------------------------------------------------------------------------------

		//用于 extended core

		template<template<typename, typename> typename HomogeneousRelation, index_type Index1, index_type Index2>
		static constexpr bool solvable_trait_homogeneous_relation = requires
		{
			typename HomogeneousRelation<
				typename List::template element<Index1>,
				typename List::template element<Index2>
			>;
		};

		template<template<typename, typename> typename HomogeneousRelation, index_type ProbeIndex1, index_type ProbeIndex2>
		struct trait_homogeneous_relation_solver
		{
			using TraitCertificate = HomogeneousRelation<
				typename List::template element<ProbeIndex1>,
				typename List::template element<ProbeIndex2>
			>;

			static constexpr auto SolverProxy{ detail::trait_predicate_solver<TraitCertificate> };

			template<index_type Index1, index_type Index2>
			static constexpr bool solve()
			{
				return SolverProxy.template operator() < HomogeneousRelation<
					typename List::template element<Index1>,
					typename List::template element<Index2>
				> > ();
			}
		};

		//操作 operation================================================================================

		template<typename OperationType, index_type Index>
		static constexpr bool executable_operation_type = requires(OperationType && operation)
		{
			std::forward<OperationType>(operation).template operator() <
				typename List::template element<Index>
			> ();
		};

		template<index_type Index, typename OperationType>
		static constexpr void execute_operation(OperationType&& operation)
		{
			std::forward<OperationType>(operation).template operator() <
				typename List::template element<Index>
			> ();
		}

		//带索引操作 operation with index================================================================================

		template<typename OperationType, index_type Index>
		static constexpr bool executable_operation_with_index_type = requires(OperationType && operation)
		{
			std::forward<OperationType>(operation).template operator() <
				Index,
				typename List::template element<Index>
			> ();
		};

		template<index_type Index, typename OperationType>
		static constexpr void execute_operation_with_index(OperationType&& operation)
		{
			std::forward<OperationType>(operation).template operator() <
				Index,
				typename List::template element<Index>
			> ();
		}

		//组合子 combinator================================================================================

		template<typename Combinator>
		struct to_type_combinator_folder
		{
			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename decltype(std::remove_reference_t<Combinator>{}.template operator() <
					Accumulator,
					typename List::template element<Index>
				> ())::type;
			};

			template<typename Accumulator, index_type Index>
			using fold =
				typename decltype(std::remove_reference_t<Combinator>{}.template operator() <
					Accumulator,
					typename List::template element<Index>
				> ())::type;
		};

		//--------------------------------------------------------------------------------

		template<typename Combinator>
		struct to_value_combinator_folder
		{
			template<auto Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				std::remove_reference_t<Combinator>{}.template operator() <
					Accumulator,
					typename List::template element<Index>
				> ();
			};

			template<auto Accumulator, index_type Index>
			static constexpr auto fold()
			{
				return std::remove_reference_t<Combinator>{}.template operator() <
					Accumulator,
					typename List::template element<Index>
				> ();
			}
		};

		//--------------------------------------------------------------------------------

		//用于 extended core

		template<template<typename Accumulator, typename Type> typename Combinator>
		struct alias_to_type_combinator_folder
		{
			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename Combinator<
					Accumulator,
					typename List::template element<Index>
				>;
			};

			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Accumulator,
				typename List::template element<Index>
			>;
		};

		//带索引组合子 combinator with index================================================================================

		template<typename Combinator>
		struct to_type_combinator_with_index_folder
		{
			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename decltype(std::remove_reference_t<Combinator>{}.template operator() <
					Index,
					Accumulator,
					typename List::template element<Index>
				> ())::type;
			};

			template<typename Accumulator, index_type Index>
			using fold =
				typename decltype(std::remove_reference_t<Combinator>{}.template operator() <
					Index,
					Accumulator,
					typename List::template element<Index>
				> ())::type;
		};

		//--------------------------------------------------------------------------------

		template<typename Combinator>
		struct to_value_combinator_with_index_folder
		{
			template<auto Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				std::remove_reference_t<Combinator>{}.template operator() <
					Index,
					Accumulator,
					typename List::template element<Index>
				> ();
			};

			template<auto Accumulator, index_type Index>
			static constexpr auto fold()
			{
				return std::remove_reference_t<Combinator>{}.template operator() <
					Index,
					Accumulator,
					typename List::template element<Index>
				> ();
			}
		};

		//--------------------------------------------------------------------------------

		//用于 extended core

		template<template<auto Index, typename Accumulator, typename Type> typename Combinator>
		struct alias_to_type_combinator_with_index_folder
		{
			template<typename Accumulator, index_type Index>
			static constexpr bool foldable = requires
			{
				typename Combinator<
					Index,
					Accumulator,
					typename List::template element<Index>
				>;
			};

			template<typename Accumulator, index_type Index>
			using fold = Combinator<
				Index,
				Accumulator,
				typename List::template element<Index>
			>;
		};

	};

}

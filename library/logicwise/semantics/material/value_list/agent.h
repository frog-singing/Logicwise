// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>

#include <logicwise/index/type.h>
#include <type_traits> //用于 std::remove_reference_t
#include <utility> //用于 std::forward


//逻辑维度::细节
namespace logicwise::detail
{
	//语义::物料 semantics::material================================================================================

	template<typename List>
	struct value_list_agent
	{
		using index_trait = IndexTraitScalar;
		using index_type = typename index_trait::index_type;

		template<typename Predicate>
		struct predicate_solver
		{
			template<index_type Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					List::template element<Index>
				> () };
			};

			template<index_type Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					List::template element<Index>
				> ();
			}
		};

		template<typename PredicateType, index_type Index>
		static constexpr bool solvable_predicate_type = requires(PredicateType&& predicate)
		{
			bool{ std::forward<PredicateType>(predicate).template operator() <
				List::template element<Index>
			> () };
		};

		template<index_type Index, typename PredicateType>
		static constexpr bool solve_predicate(PredicateType&& predicate)
		{
			return std::forward<PredicateType>(predicate).template operator() <
				List::template element<Index>
			> ();
		}

		template<typename Predicate>
		struct predicate_with_index_solver
		{
			template<index_type Index>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					List::template element<Index>
				> () };
			};

			template<index_type Index>
			static constexpr bool solve()
			{
				return std::remove_reference_t<Predicate>{}.template operator() <
					Index,
					List::template element<Index>
				> ();
			}
		};

		template<typename PredicateType, index_type Index>
		static constexpr bool solvable_predicate_with_index_type = requires(PredicateType&& predicate)
		{
			bool{ std::forward<PredicateType>(predicate).template operator() <
				Index,
				List::template element<Index>
			> () };
		};

		template<index_type Index, typename PredicateType>
		static constexpr bool solve_predicate_with_index(PredicateType&& predicate)
		{
			return std::forward<PredicateType>(predicate).template operator() <
				Index,
				List::template element<Index>
			> ();
		}

		template<typename HomogeneousRelation>
		struct homogeneous_relation_solver
		{
			template<index_type Index1, index_type Index2>
			static constexpr bool solvable = requires
			{
				bool{ std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					List::template element<Index1>,
					List::template element<Index2>
				> () };
			};

			template<index_type Index1, index_type Index2>
			static constexpr bool solve()
			{
				return std::remove_reference_t<HomogeneousRelation>{}.template operator() <
					List::template element<Index1>,
					List::template element<Index2>
				> ();
			}
		};

		template<typename HomogeneousRelationType, index_type Index1, index_type Index2>
		static constexpr bool solvable_homogeneous_relation_type = requires(HomogeneousRelationType&& homogeneous_relation)
		{
			bool{ std::forward<HomogeneousRelationType>(homogeneous_relation).template operator() <
				List::template element<Index1>,
				List::template element<Index2>
			> () };
		};

		template<index_type Index1, index_type Index2, typename HomogeneousRelationType>
		static constexpr bool solve_homogeneous_relation(HomogeneousRelationType&& homogeneous_relation)
		{
			return std::forward<HomogeneousRelationType>(homogeneous_relation).template operator() <
				List::template element<Index1>,
				List::template element<Index2>
			> ();
		}

		template<typename OperationType, index_type Index>
		static constexpr bool executable_operation_type = requires(OperationType && operation)
		{
			std::forward<OperationType>(operation).template operator() <
				List::template element<Index>
			> ();
		};

		template<index_type Index, typename OperationType>
		static constexpr void execute_operation(OperationType&& operation)
		{
			std::forward<OperationType>(operation).template operator() <
				List::template element<Index>
			> ();
		}

		template<typename OperationType, index_type Index>
		static constexpr bool executable_operation_with_index_type = requires(OperationType && operation)
		{
			std::forward<OperationType>(operation).template operator() <
				Index,
				List::template element<Index>
			> ();
		};

		template<index_type Index, typename OperationType>
		static constexpr void execute_operation_with_index(OperationType&& operation)
		{
			std::forward<OperationType>(operation).template operator() <
				Index,
				List::template element<Index>
			> ();
		}

	};

}

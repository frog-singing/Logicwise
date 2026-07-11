// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "non_dependent_view_adaptor.h"
#include <cstddef> //用于 std::size_t


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename ViewProtocol, auto Data>
	struct concrete_view : ViewProtocol::template materializer<Data>
	{
		using material_agent			= typename ViewProtocol::material_agent;
		using dependent_view_adaptor	= typename ViewProtocol::dependent_view_adaptor;
		using error_reporter			= typename ViewProtocol::error_reporter;

		template<typename Adaptor>
		using new_view = concrete_view< ViewProtocol, Adaptor::template adapt<Data>() >;

		using CurrentView = concrete_view;

		struct private_constraint
		{
			template<typename Predicate>
			static constexpr bool valid_predicate_for_first_element =
				(Data.size == 0) || requires
			{
				requires material_agent::template predicate_solver<Predicate>::template solvable<Data[0]>;
			};

			template<typename Predicate>
			static constexpr bool valid_predicate_for_last_element =
				(Data.size == 0) || requires
			{
				requires material_agent::template predicate_solver<Predicate>::template solvable<Data[Data.size - 1]>;
			};

			template<typename Predicate>
			static constexpr bool valid_predicate_with_index =
				(Data.size == 0) || requires
			{
				requires material_agent::template predicate_with_index_solver<Predicate>::template solvable<Data[0]>;
			};

			template<typename HomogeneousRelation>
			static constexpr bool valid_homogeneous_relation =
				(Data.size == 0) || requires
			{
				requires material_agent::template homogeneous_relation_solver<HomogeneousRelation>::template solvable<Data[0], Data[0]>;
			};

		};

		//--------------------------------------------------------------------------------

		static constexpr auto reverse()
		{
			return new_view< non_dependent_view_adaptor::reverse >{};
		}

		//--------------------------------------------------------------------------------

		template<std::size_t Count>
		static constexpr auto take()
		{
			return new_view< non_dependent_view_adaptor::take<Count> >{};
		}

		template<std::size_t Count>
		static constexpr auto drop()
		{
			return new_view< non_dependent_view_adaptor::drop<Count> >{};
		}

		template<std::size_t Count>
		static constexpr auto take_last()
		{
			return new_view< non_dependent_view_adaptor::take_last<Count> >{};
		}

		template<std::size_t Count>
		static constexpr auto drop_last()
		{
			return new_view< non_dependent_view_adaptor::drop_last<Count> >{};
		}

		template<std::size_t Start, std::size_t End>
		static constexpr auto slice()
		{
			return new_view< non_dependent_view_adaptor::slice<Start, End> >{};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto take_while(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template take_while<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto take_until(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template take_until<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto drop_while(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template drop_while<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto drop_until(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template drop_until<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_last_element<Predicate>
		static constexpr auto take_last_while(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template take_last_while<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_last_element<Predicate>
		static constexpr auto take_last_until(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template take_last_until<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_last_element<Predicate>
		static constexpr auto drop_last_while(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template drop_last_while<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_last_element<Predicate>
		static constexpr auto drop_last_until(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template drop_last_until<Predicate> >{};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto select(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template select<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_for_first_element<Predicate>
		static constexpr auto remove(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template remove<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_with_index<Predicate>
		static constexpr auto select_with_index(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template select_with_index<Predicate> >{};
		}

		template<typename Predicate>
			requires private_constraint::template valid_predicate_with_index<Predicate>
		static constexpr auto remove_with_index(Predicate&&)
		{
			return new_view< typename dependent_view_adaptor::template remove_with_index<Predicate> >{};
		}

		//--------------------------------------------------------------------------------

		template<typename EquivalenceRelation>
			requires private_constraint::template valid_homogeneous_relation<EquivalenceRelation>
		static constexpr auto unique_first(EquivalenceRelation&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_first<EquivalenceRelation> >{};
		}

		template<typename EquivalenceRelation>
			requires private_constraint::template valid_homogeneous_relation<EquivalenceRelation>
		static constexpr auto unique_last(EquivalenceRelation&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_last<EquivalenceRelation> >{};
		}

		template<typename EquivalenceRelation>
			requires private_constraint::template valid_homogeneous_relation<EquivalenceRelation>
		static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_adjacent_first<EquivalenceRelation> >{};
		}

		template<typename EquivalenceRelation>
			requires private_constraint::template valid_homogeneous_relation<EquivalenceRelation>
		static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_adjacent_last<EquivalenceRelation> >{};
		}

		//--------------------------------------------------------------------------------

		//严格弱序关系(Strict Weak Order)需要满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)
		//以及不可比关系的传递性(Transitivity of Incomparability)。
		//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
		template<typename StrictWeakOrder>
			requires private_constraint::template valid_homogeneous_relation<StrictWeakOrder>
		static constexpr auto sort(StrictWeakOrder&&)
		{
			return new_view< typename dependent_view_adaptor::template sort<StrictWeakOrder> >{};
		}

		//--------------------------------------------------------------------------------

		//偏序关系(Partial Order)原则上要求所有元素满足自反性(Reflexivity)，但是 sort_with_partial_order 会把反自反的元素按原顺序放在最后。
		//除此之外偏序关系(Partial Order)还要满足反对称性(Antisymmetry)、传递性(Transitivity)。
		template<typename PartialOrder>
			requires private_constraint::template valid_homogeneous_relation<PartialOrder>
		static constexpr auto sort_with_partial_order(PartialOrder&&)
		{
			return new_view< typename dependent_view_adaptor::template sort_with_partial_order<PartialOrder> >{};
		}

		template<typename PartialOrder>
			requires private_constraint::template valid_homogeneous_relation<PartialOrder>
		static constexpr auto unique_first_with_partial_order(PartialOrder&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_first_with_partial_order<PartialOrder> >{};
		}

		template<typename PartialOrder>
			requires private_constraint::template valid_homogeneous_relation<PartialOrder>
		static constexpr auto unique_last_with_partial_order(PartialOrder&&)
		{
			return new_view< typename dependent_view_adaptor::template unique_last_with_partial_order<PartialOrder> >{};
		}

		//--------------------------------------------------------------------------------

		template<typename Predicate>
		static constexpr auto take_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_last_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto take_last_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_last_while(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto drop_last_until(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto select(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto remove(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto select_with_index(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate_with_index<Predicate>();

			return CurrentView{};
		}

		template<typename Predicate>
		static constexpr auto remove_with_index(Predicate&&)
		{
			error_reporter::template incompatible_non_capturing_predicate_with_index<Predicate>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_first(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_last(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename EquivalenceRelation>
		static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
		{
			error_reporter::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

			return CurrentView{};
		}

		template<typename StrictWeakOrder>
		static constexpr auto sort(StrictWeakOrder&&)
		{
			error_reporter::template incompatible_non_capturing_strict_weak_order<StrictWeakOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto sort_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto unique_first_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

		template<typename PartialOrder>
		static constexpr auto unique_last_with_partial_order(PartialOrder&&)
		{
			error_reporter::template incompatible_non_capturing_partial_order<PartialOrder>();

			return CurrentView{};
		}

	};

}

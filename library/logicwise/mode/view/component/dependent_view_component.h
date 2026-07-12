// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::视图化 mode::view================================================================================

	template<typename ViewPipeline>
	struct dependent_view_component
	{
		using constraint				= typename ViewPipeline::constraint;
		using dependent_view_adaptor	= typename ViewPipeline::dependent_view_adaptor;
		using error_reporter			= typename ViewPipeline::error_reporter;

		template<typename Adaptor>
		using new_view = ViewPipeline::template new_view<Adaptor>;

		using CurrentView = ViewPipeline::view;

		struct component
		{
			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto take_while(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template take_while<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto take_until(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template take_until<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto drop_while(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template drop_while<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto drop_until(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template drop_until<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_last_element<Predicate>
			static constexpr auto take_last_while(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template take_last_while<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_last_element<Predicate>
			static constexpr auto take_last_until(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template take_last_until<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_last_element<Predicate>
			static constexpr auto drop_last_while(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template drop_last_while<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_last_element<Predicate>
			static constexpr auto drop_last_until(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template drop_last_until<Predicate> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto select(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template select<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_for_first_element<Predicate>
			static constexpr auto remove(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template remove<Predicate> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
				requires constraint::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_first(EquivalenceRelation&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_first<EquivalenceRelation> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_last(EquivalenceRelation&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_last<EquivalenceRelation> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_adjacent_first<EquivalenceRelation> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_adjacent_last<EquivalenceRelation> >{};
			}

			//--------------------------------------------------------------------------------

			//严格弱序关系(Strict Weak Order)要求元素满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)，
			//以及不可比关系的传递性(Transitivity of Incomparability)。
			//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
			template<typename StrictWeakOrder>
				requires constraint::template valid_homogeneous_relation<StrictWeakOrder>
			static constexpr auto sort(StrictWeakOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template sort<StrictWeakOrder> >{};
			}

			//--------------------------------------------------------------------------------

			//部分弱序关系(Partial Weak Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足传递性(Transitivity)和连通性(Connectedness)。
			//sort_with_partial_weak_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			template<typename PartialWeakOrder>
				requires constraint::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template sort_with_partial_weak_order<PartialWeakOrder> >{};
			}

			//unique_first_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_first_with_partial_weak_order<PartialWeakOrder> >{};
			}

			//unique_last_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template unique_last_with_partial_weak_order<PartialWeakOrder> >{};
			}

			//--------------------------------------------------------------------------------

			//严格偏序关系(Strict Partial Order)要求元素满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)。
			template<typename StrictPartialOrder>
				requires constraint::template valid_homogeneous_relation<StrictPartialOrder>
			static constexpr auto sort_with_strict_partial_order(StrictPartialOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template sort_with_strict_partial_order<StrictPartialOrder> >{};
			}

			//--------------------------------------------------------------------------------

			//部分偏序关系(Partial Partial Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足反对称性(Antisymmetry)和传递性(Transitivity)。
			//sort_with_partial_partial_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			template<typename PartialPartialOrder>
				requires constraint::template valid_homogeneous_relation<PartialPartialOrder>
			static constexpr auto sort_with_partial_partial_order(PartialPartialOrder&&)
			{
				return new_view< typename dependent_view_adaptor::template sort_with_partial_partial_order<PartialPartialOrder> >{};
			}

			//================================================================================

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

			//--------------------------------------------------------------------------------

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

			//--------------------------------------------------------------------------------

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

			//--------------------------------------------------------------------------------

			template<typename StrictWeakOrder>
			static constexpr auto sort(StrictWeakOrder&&)
			{
				error_reporter::template incompatible_non_capturing_strict_weak_order<StrictWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialWeakOrder>
			static constexpr auto sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename StrictPartialOrder>
			static constexpr auto sort_with_strict_partial_order(StrictPartialOrder&&)
			{
				error_reporter::template incompatible_non_capturing_strict_partial_order<StrictPartialOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialPartialOrder>
			static constexpr auto sort_with_partial_partial_order(PartialPartialOrder&&)
			{
				error_reporter::template incompatible_non_capturing_partial_partial_order<PartialPartialOrder>();

				return CurrentView{};
			}

		};

		struct component_with_index
		{
			template<typename Predicate>
				requires constraint::template valid_predicate_with_index<Predicate>
			static constexpr auto select_with_index(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template select_with_index<Predicate> >{};
			}

			template<typename Predicate>
				requires constraint::template valid_predicate_with_index<Predicate>
			static constexpr auto remove_with_index(Predicate&&)
			{
				return new_view< typename dependent_view_adaptor::template remove_with_index<Predicate> >{};
			}

			//================================================================================

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

		};

	};

}

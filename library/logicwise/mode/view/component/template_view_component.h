// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/view/adaptor/template_view_adaptor.h>
#include <cstddef> //用于 std::size_t


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::视图 mode::view================================================================================

	template<typename ViewWorkshop>
	struct template_view_component
	{
		using view_adaptor = template_view_adaptor;

		//--------------------------------------------------------------------------------

		using material_agent	= typename ViewWorkshop::material_agent;
		using constraint		= typename ViewWorkshop::constraint;
		using error_reporter	= typename ViewWorkshop::error_reporter;

		template<typename Adaptor>
		using new_view = typename ViewWorkshop::template new_view<Adaptor>;

		using CurrentView = typename ViewWorkshop::view;

		//--------------------------------------------------------------------------------

		using computation_operator = typename material_agent::computation_operator;

		//================================================================================

		struct elementwise
		{
			static constexpr auto reverse()
			{
				return new_view< view_adaptor::reverse >{};
			}

			//--------------------------------------------------------------------------------

			template<std::size_t Count>
			static constexpr auto take()
			{
				return new_view< view_adaptor::take<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto drop()
			{
				return new_view< view_adaptor::drop<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto take_last()
			{
				return new_view< view_adaptor::take_last<Count> >{};
			}

			template<std::size_t Count>
			static constexpr auto drop_last()
			{
				return new_view< view_adaptor::drop_last<Count> >{};
			}

			template<std::size_t Start, std::size_t End>
			static constexpr auto slice()
			{
				return new_view< view_adaptor::slice<Start, End> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto take_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_while<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto take_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_until<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto drop_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_while<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto drop_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_until<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto take_last_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_last_while<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto take_last_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_last_until<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto drop_last_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_last_while<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto drop_last_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_last_until<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto select(Predicate&&)
			{
				return new_view< typename view_adaptor::template select<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto remove(Predicate&&)
			{
				return new_view< typename view_adaptor::template remove<
					typename computation_operator::elementwise::template type_predicate_solver<Predicate>
				> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
				requires constraint::elementwise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_first(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_first<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::elementwise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_last(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_last<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::elementwise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_adjacent_first<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::elementwise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_adjacent_last<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//严格弱序关系(Strict Weak Order)要求元素满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)，
			//以及不可比关系的传递性(Transitivity of Incomparability)。
			//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
			template<typename StrictWeakOrder>
				requires constraint::elementwise::template valid_homogeneous_relation<StrictWeakOrder>
			static constexpr auto sort(StrictWeakOrder&&)
			{
				return new_view< typename view_adaptor::template sort<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<StrictWeakOrder>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//部分弱序关系(Partial Weak Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足传递性(Transitivity)和连通性(Connectedness)。
			//sort_with_partial_weak_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			template<typename PartialWeakOrder>
				requires constraint::elementwise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template sort_with_partial_weak_order<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//unique_first_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::elementwise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template unique_first_with_partial_weak_order<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//unique_last_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::elementwise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template unique_last_with_partial_weak_order<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//无环关系(Acyclic Relation)要求元素关系形成有向无环图(Directed Acyclic Graph)。
			template<typename AcyclicRelation>
				requires constraint::elementwise::template valid_homogeneous_relation<AcyclicRelation>
			static constexpr auto sort_topologically(AcyclicRelation&&)
			{
				return new_view< typename view_adaptor::template sort_topologically<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<AcyclicRelation>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//部分偏序关系(Partial Partial Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足反对称性(Antisymmetry)和传递性(Transitivity)。
			//sort_topologically_with_partial_partial_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			//实际上这里的关系，只需要当元素满足自反性时，满足反对称性即可，不需要满足传递性；
			//但是这种关系在学术和工程上的定义都不够直观，故而采用更严格的部分偏序关系来约束。
			template<typename PartialPartialOrder>
				requires constraint::elementwise::template valid_homogeneous_relation<PartialPartialOrder>
			static constexpr auto sort_topologically_with_partial_partial_order(PartialPartialOrder&&)
			{
				return new_view< typename view_adaptor::template sort_topologically_with_partial_partial_order<
					typename computation_operator::elementwise::template type_homogeneous_relation_solver<PartialPartialOrder>
				> >{};
			}

			//================================================================================

			template<typename Predicate>
			static constexpr auto take_while(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_until(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_while(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_until(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_last_while(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto take_last_until(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_last_while(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto drop_last_until(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
			static constexpr auto select(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto remove(Predicate&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
			static constexpr auto unique_first(EquivalenceRelation&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_last(EquivalenceRelation&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_adjacent_first(EquivalenceRelation&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto unique_adjacent_last(EquivalenceRelation&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename StrictWeakOrder>
			static constexpr auto sort(StrictWeakOrder&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_strict_weak_order<StrictWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialWeakOrder>
			static constexpr auto sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename AcyclicRelation>
			static constexpr auto sort_topologically(AcyclicRelation&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_acyclic_relation<AcyclicRelation>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialPartialOrder>
			static constexpr auto sort_topologically_with_partial_partial_order(PartialPartialOrder&&)
			{
				error_reporter::elementwise::template incompatible_non_capturing_partial_partial_order<PartialPartialOrder>();

				return CurrentView{};
			}

		};

		struct elementwise_with_index
		{
			template<typename Predicate>
				requires constraint::elementwise_with_index::template valid_predicate<Predicate>
			static constexpr auto select_with_index(Predicate&&)
			{
				return new_view< typename view_adaptor::template select<
					typename computation_operator::elementwise_with_index::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::elementwise_with_index::template valid_predicate<Predicate>
			static constexpr auto remove_with_index(Predicate&&)
			{
				return new_view< typename view_adaptor::template remove<
					typename computation_operator::elementwise_with_index::template type_predicate_solver<Predicate>
				> >{};
			}

			//================================================================================

			template<typename Predicate>
			static constexpr auto select_with_index(Predicate&&)
			{
				error_reporter::elementwise_with_index::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto remove_with_index(Predicate&&)
			{
				error_reporter::elementwise_with_index::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

		};

		struct rangewise
		{
			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_take_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_while<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_take_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_until<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_drop_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_while<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_drop_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_until<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto rangewise_take_last_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_last_while<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto rangewise_take_last_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template take_last_until<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto rangewise_drop_last_while(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_last_while<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_last_element<Predicate>
			static constexpr auto rangewise_drop_last_until(Predicate&&)
			{
				return new_view< typename view_adaptor::template drop_last_until<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_select(Predicate&&)
			{
				return new_view< typename view_adaptor::template select<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise::template valid_predicate_for_first_element<Predicate>
			static constexpr auto rangewise_remove(Predicate&&)
			{
				return new_view< typename view_adaptor::template remove<
					typename computation_operator::rangewise::template type_predicate_solver<Predicate>
				> >{};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
				requires constraint::rangewise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto rangewise_unique_first(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_first<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::rangewise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto rangewise_unique_last(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_last<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::rangewise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto rangewise_unique_adjacent_first(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_adjacent_first<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			template<typename EquivalenceRelation>
				requires constraint::rangewise::template valid_homogeneous_relation<EquivalenceRelation>
			static constexpr auto rangewise_unique_adjacent_last(EquivalenceRelation&&)
			{
				return new_view< typename view_adaptor::template unique_adjacent_last<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<EquivalenceRelation>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//严格弱序关系(Strict Weak Order)要求元素满足反自反性(Irreflexivity)、非对称性(Asymmetry)、传递性(Transitivity)，
			//以及不可比关系的传递性(Transitivity of Incomparability)。
			//只满足前三种性质的关系为严格偏序关系(Strict Partial Order)，它的不可比关系不一定具备传递性。
			template<typename StrictWeakOrder>
				requires constraint::rangewise::template valid_homogeneous_relation<StrictWeakOrder>
			static constexpr auto rangewise_sort(StrictWeakOrder&&)
			{
				return new_view< typename view_adaptor::template sort<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<StrictWeakOrder>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//部分弱序关系(Partial Weak Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足传递性(Transitivity)和连通性(Connectedness)。
			//sort_with_partial_weak_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			template<typename PartialWeakOrder>
				requires constraint::rangewise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto rangewise_sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template sort_with_partial_weak_order<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//unique_first_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::rangewise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto rangewise_unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template unique_first_with_partial_weak_order<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//unique_last_with_partial_weak_order 会忽略反自反(irreflexive)的元素。
			template<typename PartialWeakOrder>
				requires constraint::rangewise::template valid_homogeneous_relation<PartialWeakOrder>
			static constexpr auto rangewise_unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				return new_view< typename view_adaptor::template unique_last_with_partial_weak_order<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<PartialWeakOrder>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//无环关系(Acyclic Relation)要求元素关系形成有向无环图(Directed Acyclic Graph)。
			template<typename AcyclicRelation>
				requires constraint::rangewise::template valid_homogeneous_relation<AcyclicRelation>
			static constexpr auto rangewise_sort_topologically(AcyclicRelation&&)
			{
				return new_view< typename view_adaptor::template sort_topologically<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<AcyclicRelation>
				> >{};
			}

			//--------------------------------------------------------------------------------

			//部分偏序关系(Partial Partial Order)要求，当元素满足自反性(Reflexivity)时，
			//需要同时满足反对称性(Antisymmetry)和传递性(Transitivity)。
			//sort_topologically_with_partial_partial_order 会把反自反(irreflexive)的元素按原顺序放在最后。
			//实际上这里的关系，只需要当元素满足自反性时，满足反对称性即可，不需要满足传递性；
			//但是这种关系在学术和工程上的定义都不够直观，故而采用更严格的部分偏序关系来约束。
			template<typename PartialPartialOrder>
				requires constraint::rangewise::template valid_homogeneous_relation<PartialPartialOrder>
			static constexpr auto rangewise_sort_topologically_with_partial_partial_order(PartialPartialOrder&&)
			{
				return new_view< typename view_adaptor::template sort_topologically_with_partial_partial_order<
					typename computation_operator::rangewise::template type_homogeneous_relation_solver<PartialPartialOrder>
				> >{};
			}

			//================================================================================

			template<typename Predicate>
			static constexpr auto rangewise_take_while(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_take_until(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_drop_while(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_drop_until(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_take_last_while(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_take_last_until(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_drop_last_while(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_drop_last_until(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename Predicate>
			static constexpr auto rangewise_select(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_remove(Predicate&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename EquivalenceRelation>
			static constexpr auto rangewise_unique_first(EquivalenceRelation&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto rangewise_unique_last(EquivalenceRelation&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto rangewise_unique_adjacent_first(EquivalenceRelation&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			template<typename EquivalenceRelation>
			static constexpr auto rangewise_unique_adjacent_last(EquivalenceRelation&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_equivalence_relation<EquivalenceRelation>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename StrictWeakOrder>
			static constexpr auto rangewise_sort(StrictWeakOrder&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_strict_weak_order<StrictWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialWeakOrder>
			static constexpr auto rangewise_sort_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto rangewise_unique_first_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			template<typename PartialWeakOrder>
			static constexpr auto rangewise_unique_last_with_partial_weak_order(PartialWeakOrder&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_partial_weak_order<PartialWeakOrder>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename AcyclicRelation>
			static constexpr auto rangewise_sort_topologically(AcyclicRelation&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_acyclic_relation<AcyclicRelation>();

				return CurrentView{};
			}

			//--------------------------------------------------------------------------------

			template<typename PartialPartialOrder>
			static constexpr auto rangewise_sort_topologically_with_partial_partial_order(PartialPartialOrder&&)
			{
				error_reporter::rangewise::template incompatible_non_capturing_partial_partial_order<PartialPartialOrder>();

				return CurrentView{};
			}

		};

		struct rangewise_with_index
		{
			template<typename Predicate>
				requires constraint::rangewise_with_index::template valid_predicate<Predicate>
			static constexpr auto rangewise_select_with_index(Predicate&&)
			{
				return new_view< typename view_adaptor::template select<
					typename computation_operator::rangewise_with_index::template type_predicate_solver<Predicate>
				> >{};
			}

			template<typename Predicate>
				requires constraint::rangewise_with_index::template valid_predicate<Predicate>
			static constexpr auto rangewise_remove_with_index(Predicate&&)
			{
				return new_view< typename view_adaptor::template remove<
					typename computation_operator::rangewise_with_index::template type_predicate_solver<Predicate>
				> >{};
			}

			//================================================================================

			template<typename Predicate>
			static constexpr auto rangewise_select_with_index(Predicate&&)
			{
				error_reporter::rangewise_with_index::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

			template<typename Predicate>
			static constexpr auto rangewise_remove_with_index(Predicate&&)
			{
				error_reporter::rangewise_with_index::template incompatible_non_capturing_predicate<Predicate>();

				return CurrentView{};
			}

		};

	};

}

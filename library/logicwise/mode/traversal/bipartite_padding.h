// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/arrangement/type.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/vector_like_container.h>
#include <logicwise/semantics/padding/template_element_padder.h>
#include <logicwise/semantics/padding/padding_instance_trait.h>
#include "traversal_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class bipartite_traversal;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::遍历 mode::traversal================================================================================

	//(PaddingElementI, PaddingElementJ) 必须是合法组合

	template<typename Mode, ArrangementWithPadding Arrangement>
	class bipartite_traversal<Mode, Arrangement>
	{
	public:
		template<typename TypeA, typename TypeB>
		[[nodiscard]] static constexpr auto with_padding()
		{
			return pad_with_type<TypeA, TypeB>{};
		}

		template<auto ValueA, auto ValueB>
		[[nodiscard]] static constexpr auto with_padding()
		{
			return pad_with_value<ValueA, ValueB>{};
		}

		template<typename Type, auto Value>
		[[nodiscard]] static constexpr auto with_padding()
		{
			return pad_with_type_and_value<Type, Value>{};
		}

		template<auto Value, typename Type>
		[[nodiscard]] static constexpr auto with_padding()
		{
			return pad_with_value_and_type<Value, Type>{};
		}

		//--------------------------------------------------------------------------------

		template<typename InstanceTypeA, typename InstanceTypeB>
		[[nodiscard]] static constexpr auto with_padding(InstanceTypeA&& instanceA, InstanceTypeB&& instanceB)
		{
			return pad_with_instance<InstanceTypeA, InstanceTypeB>
			{ std::forward<InstanceTypeA>(instanceA), std::forward<InstanceTypeB>(instanceB) };
		}

		//--------------------------------------------------------------------------------

		template<typename Type, typename InstanceType>
		[[nodiscard]] static constexpr auto with_padding(InstanceType&& instance)
		{
			return pad_with_type_and_instance<Type, InstanceType>
			{ std::forward<InstanceType>(instance) };
		}

		template<auto Value, typename InstanceType>
		[[nodiscard]] static constexpr auto with_padding(InstanceType&& instance)
		{
			return pad_with_value_and_instance<Value, InstanceType>
			{ std::forward<InstanceType>(instance) };
		}


	private:
		using extent_type = typename Arrangement::extent_type;
		using IndexTraverserType = typename Mode::template index_traverser<Arrangement>;
		using ActualIndexType = decltype(std::declval<IndexTraverserType>().state());

		template<typename PaddingTypeI, typename PaddingTypeJ>
		class pad_with_type
		{
		public:
			template<typewise::List ListA, typewise::List ListB>
			[[nodiscard]] static constexpr auto between()
			{
				return between_type_list<as_type_list<ListA>, as_type_list<ListB>>{};
			}

			template<typename WrapperInstanceA, typename WrapperInstanceB>
			static constexpr auto between()
			{
				static_assert(dependent_false_v<WrapperInstanceA, WrapperInstanceB>,
					"[logicwise] Error: Incompatible <WrapperInstance>!\n"
					"Expected: a pure type list and a pure type list.");

				return false;
			}

		private:
			template<typename ListA, typename ListB>
			struct between_type_list
			{
				static constexpr extent_type Extent{ ListA::size, ListB::size };

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingTypeJ > ();
				}
				static constexpr void execute(OperationType&& operation)
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingTypeJ > () };
				}
				static constexpr void execute_until(OperationType&& operation)
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingTypeJ > () };
				}
				static constexpr void execute_while(OperationType&& operation)
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, typename TypeJ>() { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, typename TypeJ>() -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, typename TypeJ>() -> bool { ... }");
				}

			};

		};

		template<auto PaddingValueI, auto PaddingValueJ>
		class pad_with_value
		{
		public:
			template<valuewise::List ListA, valuewise::List ListB>
			[[nodiscard]] static constexpr auto between()
			{
				return between_value_list<as_value_list<ListA>, as_value_list<ListB>>{};
			}

			template<typename WrapperInstanceA, typename WrapperInstanceB>
			static constexpr auto between()
			{
				static_assert(dependent_false_v<WrapperInstanceA, WrapperInstanceB>,
					"[logicwise] Error: Incompatible <WrapperInstance>!\n"
					"Expected: a pure value list and a pure value list.");

				return false;
			}

		private:
			template<typename ListA, typename ListB>
			struct between_value_list
			{
				static constexpr extent_type Extent{ ListA::size, ListB::size };

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingValueJ > ();
				}
				static constexpr void execute(OperationType&& operation)
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingValueJ > () };
				}
				static constexpr void execute_until(OperationType&& operation)
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingValueJ > () };
				}
				static constexpr void execute_while(OperationType&& operation)
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, auto ValueJ>() { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, auto ValueJ>() -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, auto ValueJ>() -> bool { ... }");
				}

			};

		};

		template<typename PaddingTypeI, auto PaddingValueJ>
		class pad_with_type_and_value
		{
		public:
			template<typewise::List ListA, valuewise::List ListB>
			[[nodiscard]] static constexpr auto between()
			{
				return between_type_list_and_value_list<as_type_list<ListA>, as_value_list<ListB>>{};
			}

			template<typename WrapperInstanceA, typename WrapperInstanceB>
			static constexpr auto between()
			{
				static_assert(dependent_false_v<WrapperInstanceA, WrapperInstanceB>,
					"[logicwise] Error: Incompatible <WrapperInstance>!\n"
					"Expected: a pure type list and a pure value list.");

				return false;
			}

		private:
			template<typename ListA, typename ListB>
			struct between_type_list_and_value_list
			{
				static constexpr extent_type Extent{ ListA::size, ListB::size };

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingValueJ > ();
				}
				static constexpr void execute(OperationType&& operation)
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingValueJ > () };
				}
				static constexpr void execute_until(OperationType&& operation)
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI, PaddingValueJ > () };
				}
				static constexpr void execute_while(OperationType&& operation)
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								typename template_element_padder::template actual_type
								<padding_state[0], PaddingTypeI, ListA, component[0]>,
								template_element_padder::template actual_value
								<padding_state[1], PaddingValueJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, auto ValueJ>() { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, auto ValueJ>() -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI, auto ValueJ>() -> bool { ... }");
				}

			};

		};

		template<auto PaddingValueI, typename PaddingTypeJ>
		class pad_with_value_and_type
		{
		public:
			template<valuewise::List ListA, typewise::List ListB>
			[[nodiscard]] static constexpr auto between()
			{
				return between_value_list_and_type_list<as_value_list<ListA>, as_type_list<ListB>>{};
			}

			template<typename WrapperInstanceA, typename WrapperInstanceB>
			static constexpr auto between()
			{
				static_assert(dependent_false_v<WrapperInstanceA, WrapperInstanceB>,
					"[logicwise] Error: Incompatible <WrapperInstance>!\n"
					"Expected: a pure value list and a pure type list.");

				return false;
			}

		private:
			template<typename ListA, typename ListB>
			struct between_value_list_and_type_list
			{
				static constexpr extent_type Extent{ ListA::size, ListB::size };

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingTypeJ > ();
				}
				static constexpr void execute(OperationType&& operation)
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingTypeJ > () };
				}
				static constexpr void execute_until(OperationType&& operation)
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI, PaddingTypeJ > () };
				}
				static constexpr void execute_while(OperationType&& operation)
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator() <
								template_element_padder::template actual_value
								<padding_state[0], PaddingValueI, ListA, component[0]>,
								typename template_element_padder::template actual_type
								<padding_state[1], PaddingTypeJ, ListB, component[1]>
							> ();
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, typename TypeJ>() { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, typename TypeJ>() -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI, typename TypeJ>() -> bool { ... }");
				}

			};

		};

		//--------------------------------------------------------------------------------

		template<typename PaddingInstanceTypeI, typename PaddingInstanceTypeJ>
		class pad_with_instance
		{
		public:
			using StoredPaddingInstanceTypeI =
				typename padding_instance_trait<PaddingInstanceTypeI>::stored_padding_instance_type;

			using StoredPaddingInstanceTypeJ =
				typename padding_instance_trait<PaddingInstanceTypeJ>::stored_padding_instance_type;

			StoredPaddingInstanceTypeI padding_instance_I;
			StoredPaddingInstanceTypeJ padding_instance_J;

			pad_with_instance() = delete; //禁用默认构造函数

			explicit constexpr pad_with_instance(
				StoredPaddingInstanceTypeI&& padding_instance_I, StoredPaddingInstanceTypeJ&& padding_instance_J) noexcept
				: padding_instance_I{ std::forward<StoredPaddingInstanceTypeI>(padding_instance_I) }
				, padding_instance_J{ std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J) }
			{}

			template<VectorLike ContainerTypeA, VectorLike ContainerTypeB>
			[[nodiscard]] constexpr auto between(ContainerTypeA&& containerA, ContainerTypeB&& containerB) const&
			{
				return between_container<
					StoredPaddingInstanceTypeI&&,
					StoredPaddingInstanceTypeJ&&,
					ContainerTypeA,
					ContainerTypeB
				>
				{
					StoredPaddingInstanceTypeI{ padding_instance_I },
					StoredPaddingInstanceTypeJ{ padding_instance_J },
					std::forward<ContainerTypeA>(containerA),
					std::forward<ContainerTypeB>(containerB)
				};
			}

			template<VectorLike ContainerTypeA, VectorLike ContainerTypeB>
			[[nodiscard]] constexpr auto between(ContainerTypeA&& containerA, ContainerTypeB&& containerB) &
			{
				return between_container<
					const StoredPaddingInstanceTypeI&,
					const StoredPaddingInstanceTypeJ&,
					ContainerTypeA,
					ContainerTypeB
				>
				{
					static_cast<const StoredPaddingInstanceTypeI&>(padding_instance_I),
					static_cast<const StoredPaddingInstanceTypeJ&>(padding_instance_J),
					std::forward<ContainerTypeA>(containerA),
					std::forward<ContainerTypeB>(containerB)
				};
			}

			template<VectorLike ContainerTypeA, VectorLike ContainerTypeB>
			[[nodiscard]] constexpr auto between(ContainerTypeA&& containerA, ContainerTypeB&& containerB) &&
			{
				return between_container<
					StoredPaddingInstanceTypeI&&,
					StoredPaddingInstanceTypeJ&&,
					ContainerTypeA,
					ContainerTypeB
				>
				{
					std::forward<StoredPaddingInstanceTypeI>(padding_instance_I),
					std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J),
					std::forward<ContainerTypeA>(containerA),
					std::forward<ContainerTypeB>(containerB)
				};
			}

			template<typename ContainerTypeA, typename ContainerTypeB>
			static constexpr auto between(ContainerTypeA&&, ContainerTypeB&&)
			{
				static_assert(dependent_false_v<ContainerTypeA, ContainerTypeB>,
					"[logicwise] Error: Incompatible container!\n"
					"Expected: VectorLike container.");

				return false;
			}

		private:
			template<
				typename ExpectedPaddingInstanceTypeI,
				typename ExpectedPaddingInstanceTypeJ,
				typename ContainerTypeA,
				typename ContainerTypeB
			>
			struct between_container
			{
				using ContainerTraitA = vector_like_container_trait<ContainerTypeA>;

				using RawInstanceTypeA			= typename ContainerTraitA::raw_instance_type;
				using StoredInstanceTypeA		= typename ContainerTraitA::stored_instance_type;
				using StoredContainerTypeA		= typename ContainerTraitA::stored_container_type;
				using ExpectedContainerTypeA	= typename ContainerTraitA::expected_container_type;

				using PaddingInstanceTraitI = padding_instance_trait<ExpectedPaddingInstanceTypeI>;

				static constexpr bool I_A_compatibility = PaddingInstanceTraitI::template is_compatible_type<RawInstanceTypeA>;

				using NormalizedPaddingInstanceTypeI = typename PaddingInstanceTraitI
					::template normalized_padding_instance_type<RawInstanceTypeA>;

				using ContainerTraitB = vector_like_container_trait<ContainerTypeB>;

				using RawInstanceTypeB			= typename ContainerTraitB::raw_instance_type;
				using StoredInstanceTypeB		= typename ContainerTraitB::stored_instance_type;
				using StoredContainerTypeB		= typename ContainerTraitB::stored_container_type;
				using ExpectedContainerTypeB	= typename ContainerTraitB::expected_container_type;

				using PaddingInstanceTraitJ = padding_instance_trait<ExpectedPaddingInstanceTypeJ>;

				static constexpr bool J_B_compatibility = PaddingInstanceTraitJ::template is_compatible_type<RawInstanceTypeB>;
				
				using NormalizedPaddingInstanceTypeJ = typename PaddingInstanceTraitJ
					::template normalized_padding_instance_type<RawInstanceTypeB>;

				const NormalizedPaddingInstanceTypeI padding_instance_I;
				const NormalizedPaddingInstanceTypeJ padding_instance_J;

				const StoredContainerTypeA containerA;
				const StoredContainerTypeB containerB;

				between_container() = delete; //禁用默认构造函数

				explicit constexpr between_container(
					ExpectedPaddingInstanceTypeI padding_instance_I, ExpectedPaddingInstanceTypeJ padding_instance_J,
					ExpectedContainerTypeA containerA, ExpectedContainerTypeB containerB) noexcept
					requires I_A_compatibility && J_B_compatibility
					: padding_instance_I{ static_cast<ExpectedPaddingInstanceTypeI>(padding_instance_I) }
					, padding_instance_J{ static_cast<ExpectedPaddingInstanceTypeJ>(padding_instance_J) }
					, containerA{ ContainerTraitA::cast_container(static_cast<ExpectedContainerTypeA>(containerA)) }
					, containerB{ ContainerTraitB::cast_container(static_cast<ExpectedContainerTypeB>(containerB)) }
				{}

				template<typename OperationType>
					requires requires(OperationType&& operation,
				const StoredInstanceTypeA& instance_i, const StoredInstanceTypeB& instance_j)
				{
					std::invoke(std::forward<OperationType>(operation), instance_i, instance_j);
				}
				constexpr void execute(OperationType&& operation) const
				{
					extent_type extent{ std::ranges::size(containerA), std::ranges::size(containerB) };

					instance_execute_loop<Arrangement, IndexTraverserType>(extent,
						[&] (auto&& index) {
							auto&& [component, padding_state] = index;

							std::invoke(operation,
								padding_state[0] ? padding_instance_I : containerA[component[0]],
								padding_state[1] ? padding_instance_J : containerB[component[1]]);
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation,
				const StoredInstanceTypeA& instance_i, const StoredInstanceTypeB& instance_j)
				{
					bool{ std::invoke(std::forward<OperationType>(operation), instance_i, instance_j) };
				}
				constexpr void execute_until(OperationType&& operation) const
				{
					extent_type extent{ std::ranges::size(containerA), std::ranges::size(containerB) };

					instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
						[&] (auto&& index) {
							auto&& [component, padding_state] = index;

							return std::invoke(operation,
								padding_state[0] ? padding_instance_I : containerA[component[0]],
								padding_state[1] ? padding_instance_J : containerB[component[1]]);
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation,
				const StoredInstanceTypeA& instance_i, const StoredInstanceTypeB& instance_j)
				{
					bool{ std::invoke(std::forward<OperationType>(operation), instance_i, instance_j) };
				}
				constexpr void execute_while(OperationType&& operation) const
				{
					extent_type extent{ std::ranges::size(containerA), std::ranges::size(containerB) };

					instance_execute_while_loop<Arrangement, IndexTraverserType>(extent,
						[&] (auto&& index) {
							auto&& [component, padding_state] = index;

							return std::invoke(operation,
								padding_state[0] ? padding_instance_I : containerA[component[0]],
								padding_state[1] ? padding_instance_J : containerB[component[1]]);
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] (auto&& instance_i, auto&& instance_j) { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] (auto&& instance_i, auto&& instance_j) -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] (auto&& instance_i, auto&& instance_j) -> bool { ... }");
				}

			};

		};

		//--------------------------------------------------------------------------------

		template<typename PaddingTypeI, typename PaddingInstanceTypeJ>
		class pad_with_type_and_instance
		{
		public:
			using StoredPaddingInstanceTypeJ =
				typename padding_instance_trait<PaddingInstanceTypeJ>::stored_padding_instance_type;

			StoredPaddingInstanceTypeJ padding_instance_J;

			pad_with_type_and_instance() = delete; //禁用默认构造函数

			explicit constexpr pad_with_type_and_instance(StoredPaddingInstanceTypeJ&& padding_instance_J) noexcept
				: padding_instance_J{ std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J) }
			{}

			template<typewise::List TypeList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) const&
			{
				return between_type_list_and_container<
					as_type_list<TypeList>,
					StoredPaddingInstanceTypeJ&&,
					ContainerType
				>
				{
					StoredPaddingInstanceTypeJ{ padding_instance_J },
					std::forward<ContainerType>(container)
				};
			}

			template<typewise::List TypeList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) &
			{
				return between_type_list_and_container<
					as_type_list<TypeList>,
					const StoredPaddingInstanceTypeJ&,
					ContainerType
				>
				{
					static_cast<const StoredPaddingInstanceTypeJ&>(padding_instance_J),
					std::forward<ContainerType>(container)
				};
			}

			template<typewise::List TypeList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) &&
			{
				return between_type_list_and_container<
					as_type_list<TypeList>,
					StoredPaddingInstanceTypeJ&&,
					ContainerType
				>
				{
					std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J),
					std::forward<ContainerType>(container)
				};
			}

			template<typename WrapperInstance, typename ContainerType>
			static constexpr auto between(ContainerType&&)
			{
				static_assert(dependent_false_v<WrapperInstance, ContainerType>,
					"[logicwise] Error: Incompatible <WrapperInstance> or container!\n"
					"Expected <WrapperInstance>: pure type list.\n"
					"Expected container: VectorLike container.");

				return false;
			}

		private:
			template<typename List, typename ExpectedPaddingInstanceTypeJ, typename ContainerType>
			struct between_type_list_and_container
			{
				using ContainerTrait = vector_like_container_trait<ContainerType>;

				using RawInstanceType		= typename ContainerTrait::raw_instance_type;
				using StoredInstanceType	= typename ContainerTrait::stored_instance_type;
				using StoredContainerType	= typename ContainerTrait::stored_container_type;
				using ExpectedContainerType	= typename ContainerTrait::expected_container_type;

				static constexpr auto ContainerSize = static_container_size<StoredContainerType>;
				static constexpr extent_type Extent{ List::size, ContainerSize };

				using PaddingInstanceTraitJ = padding_instance_trait<ExpectedPaddingInstanceTypeJ>;

				static constexpr bool padding_instance_compatible_with_container =
					PaddingInstanceTraitJ::template is_compatible_type<RawInstanceType>;

				using NormalizedPaddingInstanceTypeJ = typename PaddingInstanceTraitJ
					::template normalized_padding_instance_type<RawInstanceType>;

				const NormalizedPaddingInstanceTypeJ padding_instance_J;

				const StoredContainerType container;

				between_type_list_and_container() = delete; //禁用默认构造函数

				explicit constexpr between_type_list_and_container(
					ExpectedPaddingInstanceTypeJ padding_instance_J, ExpectedContainerType container) noexcept
					requires padding_instance_compatible_with_container
					: padding_instance_J{ static_cast<ExpectedPaddingInstanceTypeJ>(padding_instance_J) }
					, container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) }
				{}

				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI > (instance_j);
				}
				constexpr void execute(OperationType&& operation) const
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator()
								<
									typename template_element_padder::template actual_type
									<padding_state[0], PaddingTypeI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI > (instance_j) };
				}
				constexpr void execute_until(OperationType&& operation) const
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator()
								<
									typename template_element_padder::template actual_type
									<padding_state[0], PaddingTypeI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}
				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingTypeI > (instance_j) };
				}
				constexpr void execute_while(OperationType&& operation) const
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator()
								<
									typename template_element_padder::template actual_type
									<padding_state[0], PaddingTypeI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI>(auto&& instance_j) { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI>(auto&& instance_j) -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <typename TypeI>(auto&& instance_j) -> bool { ... }");
				}

			};

		};

		template<auto PaddingValueI, typename PaddingInstanceTypeJ>
		class pad_with_value_and_instance
		{
		public:
			using StoredPaddingInstanceTypeJ =
				typename padding_instance_trait<PaddingInstanceTypeJ>::stored_padding_instance_type;

			StoredPaddingInstanceTypeJ padding_instance_J;

			pad_with_value_and_instance() = delete; //禁用默认构造函数

			explicit constexpr pad_with_value_and_instance(StoredPaddingInstanceTypeJ&& padding_instance_J) noexcept
				: padding_instance_J{ std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J) }
			{}

			template<valuewise::List ValueList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) const&
			{
				return between_value_list_and_container<
					as_value_list<ValueList>,
					StoredPaddingInstanceTypeJ&&,
					ContainerType
				>
				{
					StoredPaddingInstanceTypeJ{ padding_instance_J },
					std::forward<ContainerType>(container)
				};
			}

			template<valuewise::List ValueList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) &
			{
				return between_value_list_and_container<
					as_value_list<ValueList>,
					const StoredPaddingInstanceTypeJ&,
					ContainerType
				>
				{
					static_cast<const StoredPaddingInstanceTypeJ&>(padding_instance_J),
					std::forward<ContainerType>(container)
				};
			}

			template<valuewise::List ValueList, VectorLike ContainerType>
			[[nodiscard]] constexpr auto between(ContainerType&& container) &&
			{
				return between_value_list_and_container<
					as_value_list<ValueList>,
					StoredPaddingInstanceTypeJ&&,
					ContainerType
				>
				{
					std::forward<StoredPaddingInstanceTypeJ>(padding_instance_J),
					std::forward<ContainerType>(container)
				};
			}

			template<typename WrapperInstance, typename ContainerType>
			static constexpr auto between(ContainerType&&)
			{
				static_assert(dependent_false_v<WrapperInstance, ContainerType>,
					"[logicwise] Error: Incompatible <WrapperInstance> or container!\n"
					"Expected <WrapperInstance>: pure value list.\n"
					"Expected container: VectorLike container.");

				return false;
			}

		private:
			template<typename List, typename ExpectedPaddingInstanceTypeJ, typename ContainerType>
			struct between_value_list_and_container
			{
				using ContainerTrait = vector_like_container_trait<ContainerType>;

				using RawInstanceType		= typename ContainerTrait::raw_instance_type;
				using StoredInstanceType	= typename ContainerTrait::stored_instance_type;
				using StoredContainerType	= typename ContainerTrait::stored_container_type;
				using ExpectedContainerType	= typename ContainerTrait::expected_container_type;

				static constexpr auto ContainerSize = static_container_size<StoredContainerType>;
				static constexpr extent_type Extent{ List::size, ContainerSize };

				using PaddingInstanceTraitJ = padding_instance_trait<ExpectedPaddingInstanceTypeJ>;

				static constexpr bool padding_instance_compatible_with_container =
					PaddingInstanceTraitJ::template is_compatible_type<RawInstanceType>;

				using NormalizedPaddingInstanceTypeJ = typename PaddingInstanceTraitJ
					::template normalized_padding_instance_type<RawInstanceType>;

				const NormalizedPaddingInstanceTypeJ padding_instance_J;

				const StoredContainerType container;

				between_value_list_and_container() = delete; //禁用默认构造函数

				explicit constexpr between_value_list_and_container(
					ExpectedPaddingInstanceTypeJ padding_instance_J, ExpectedContainerType container) noexcept
					requires padding_instance_compatible_with_container
					: padding_instance_J{ static_cast<ExpectedPaddingInstanceTypeJ>(padding_instance_J) }
					, container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) }
				{}

				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					std::forward<OperationType>(operation)
						.template operator() < PaddingValueI > (instance_j);
				}
				constexpr void execute(OperationType&& operation) const
				{
					template_execute_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							operation.template operator()
								<
									template_element_padder::template actual_value
									<padding_state[0], PaddingValueI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI > (instance_j) };
				}
				constexpr void execute_until(OperationType&& operation) const
				{
					template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator()
								<
									template_element_padder::template actual_value
									<padding_state[0], PaddingValueI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}

				template<typename OperationType>
					requires requires(OperationType&& operation, const StoredInstanceType& instance_j)
				{
					bool{ std::forward<OperationType>(operation)
						.template operator() < PaddingValueI > (instance_j) };
				}
				constexpr void execute_while(OperationType&& operation) const
				{
					template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
						([&] <auto Index> {
							constexpr auto component{ Index.component };
							constexpr auto padding_state{ Index.padding_state };

							return operation.template operator()
								<
									template_element_padder::template actual_value
									<padding_state[0], PaddingValueI, List, component[0]>
								>
								(padding_state[1] ? padding_instance_J : container[component[1]]);
						});
				}

				template<typename OperationType>
				static constexpr void execute(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI>(auto&& instance_j) { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_until(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI>(auto&& instance_j) -> bool { ... }");
				}

				template<typename OperationType>
				static constexpr void execute_while(OperationType&&)
				{
					static_assert(dependent_false_v<OperationType>,
						"[logicwise] Error: Incompatible operation signature!\n"
						"Expected: [] <auto ValueI>(auto&& instance_j) -> bool { ... }");
				}

			};

		};

	};

}

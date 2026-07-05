// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/vector_like_container.h>
#include "traversal_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class pairwise_traversal;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::遍历 mode::traversal================================================================================

	template<typename Mode, typename Arrangement>
	class pairwise_traversal
	{
	public:
		template<typename WrapperInstance>
			requires typewise::List<WrapperInstance> || valuewise::List<WrapperInstance>
		[[nodiscard]] static constexpr auto in()
		{
			return in_wrapper<WrapperInstance>{};
		}

		template<VectorLike ContainerType>
		[[nodiscard]] static constexpr auto in(ContainerType&& container)
		{
			return in_container<ContainerType>{ std::forward<ContainerType>(container) };
		}

		template<typename WrapperInstance>
		static constexpr auto in()
		{
			static_assert(dependent_false_v<WrapperInstance>,
				"[logicwise] Error: Incompatible <WrapperInstance>!\n"
				"Expected: pure type list or pure value list.");

			return false;
		}

		template<typename ContainerType>
		static constexpr auto in(ContainerType&&)
		{
			static_assert(dependent_false_v<ContainerType>,
				"[logicwise] Error: Incompatible container!\n"
				"Expected: VectorLike container.");

			return false;
		}


	private:
		using extent_type = typename Arrangement::extent_type;
        using IndexTraverserType = typename Mode::template index_traverser<Arrangement>;
        using ActualIndexType = decltype(std::declval<IndexTraverserType>().state());

		template<typename WrapperInstance>
		struct in_wrapper
		{
            static constexpr void execute                   (auto&&) noexcept {}
            static constexpr void execute_with_index        (auto&&) noexcept {}
            static constexpr void execute_until             (auto&&) noexcept {}
            static constexpr void execute_until_with_index  (auto&&) noexcept {}
            static constexpr void execute_while             (auto&&) noexcept {}
            static constexpr void execute_while_with_index  (auto&&) noexcept {}

		};

		template<typewise::List TypeList>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_type_list<TypeList>::size }
			>
		struct in_wrapper<TypeList>
		{
			using List = as_type_list<TypeList>;
			static constexpr extent_type Extent{ List::size };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			using ProbeTypeI = typename List::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename List::template element<ProbeIndex[1]>;

			template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeTypeI, ProbeTypeJ > ();
            }
            static constexpr void execute(OperationType&& operation)
            {
				template_execute_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> {
                        operation.template operator() <
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
						> ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeTypeI, ProbeTypeJ > ();
            }
            static constexpr void execute_with_index(OperationType&& operation)
            {
				template_execute_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> {
                        operation.template operator() <
                            Index,
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
						> ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeTypeI, ProbeTypeJ > () };
            }
            static constexpr void execute_until(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> { return
                        operation.template operator() <
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
						> ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeTypeI, ProbeTypeJ > () };
            }
            static constexpr void execute_until_with_index(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> { return
                        operation.template operator() <
                            Index,
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
						> ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeTypeI, ProbeTypeJ > () };
            }
            static constexpr void execute_while(OperationType&& operation)
            {
                template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> { return
                        operation.template operator() <
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
						> ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeTypeI, ProbeTypeJ > () };
            }
            static constexpr void execute_while_with_index(OperationType&& operation)
            {
                template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> { return
                        operation.template operator() <
                            Index,
							typename List::template element<Index[0]>,
							typename List::template element<Index[1]>
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
            static constexpr void execute_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, typename TypeI, typename TypeJ>() { ... }");
            }

			template<typename OperationType>
			static constexpr void execute_until(OperationType&&)
			{
				static_assert(dependent_false_v<OperationType>,
					"[logicwise] Error: Incompatible operation signature!\n"
					"Expected: [] <typename TypeI, typename TypeJ>() -> bool { ... }");
			}

            template<typename OperationType>
            static constexpr void execute_until_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, typename TypeI, typename TypeJ>() -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <typename TypeI, typename TypeJ>() -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, typename TypeI, typename TypeJ>() -> bool { ... }");
            }

		};

		template<valuewise::List ValueList>
            requires TraversableExtent <
                Arrangement,
                extent_type{ as_value_list<ValueList>::size }
            >
        struct in_wrapper<ValueList>
        {
            using List = as_value_list<ValueList>;
            static constexpr extent_type Extent{ List::size };
            static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
            static constexpr auto ProbeValueI = List::template element<ProbeIndex[0]>;
            static constexpr auto ProbeValueJ = List::template element<ProbeIndex[1]>;

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeValueI, ProbeValueJ > ();
            }
            static constexpr void execute(OperationType&& operation)
            {
                template_execute_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> {
                        operation.template operator() <
							List::template element<Index[0]>,
							List::template element<Index[1]>
						> ();
                    });
            }
            
            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeValueI, ProbeValueJ > ();
            }
            static constexpr void execute_with_index(OperationType&& operation)
            {
                template_execute_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> {
                        operation.template operator() <
                            Index,
							List::template element<Index[0]>,
							List::template element<Index[1]>
						> ();
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeValueI, ProbeValueJ > () };
            }
            static constexpr void execute_until(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> { return
                        operation.template operator() <
							List::template element<Index[0]>,
							List::template element<Index[1]>
						> ();
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeValueI, ProbeValueJ > () };
            }
            static constexpr void execute_until_with_index(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> { return
                        operation.template operator() <
                            Index,
							List::template element<Index[0]>,
							List::template element<Index[1]>
						> ();
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeValueI, ProbeValueJ > () };
            }
            static constexpr void execute_while(OperationType&& operation)
            {
                template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> { return
                        operation.template operator() <
							List::template element<Index[0]>,
							List::template element<Index[1]>
						> ();
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeIndex, ProbeValueI, ProbeValueJ > () };
            }
            static constexpr void execute_while_with_index(OperationType&& operation)
            {
                template_execute_while_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> { return
                        operation.template operator() <
                            Index,
							List::template element<Index[0]>,
							List::template element<Index[1]>
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
            static constexpr void execute_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, auto ValueI, auto ValueJ>() { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto ValueI, auto ValueJ>() -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, auto ValueI, auto ValueJ>() -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto ValueI, auto ValueJ>() -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto Index, auto ValueI, auto ValueJ>() -> bool { ... }");
            }

        };


		template<typename ContainerType>
		struct in_container
		{
			using ContainerTrait = vector_like_container_trait<ContainerType>;

			using StoredInstanceType    = typename ContainerTrait::stored_instance_type;
			using StoredContainerType   = typename ContainerTrait::stored_container_type;
			using ExpectedContainerType = typename ContainerTrait::expected_container_type;

			const StoredContainerType container;

			in_container() = delete; //禁用默认构造函数

			explicit constexpr in_container(ExpectedContainerType container) noexcept
				: container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

            template<typename OperationType>
                requires requires(OperationType&& operation,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                std::invoke(std::forward<OperationType>(operation), instance_i, instance_j);
            }
            constexpr void execute(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        std::invoke(operation, container[index[0]], container[index[1]]);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index),
                    instance_i, instance_j);
            }
            constexpr void execute_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance_i = container[index[0]];
                        auto& instance_j = container[index[1]];
                        std::invoke(operation, std::forward<decltype(index)>(index), instance_i, instance_j);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), instance_i, instance_j) };
            }
            constexpr void execute_until(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) { return
                        std::invoke(operation, container[index[0]], container[index[1]]);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index),
                    instance_i, instance_j) };
            }
            constexpr void execute_until_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance_i = container[index[0]];
                        auto& instance_j = container[index[1]];
                        return std::invoke(operation, std::forward<decltype(index)>(index), instance_i, instance_j);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), instance_i, instance_j) };
            }
            constexpr void execute_while(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_while_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) { return
                        std::invoke(operation, container[index[0]], container[index[1]]);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index,
            const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index),
                    instance_i, instance_j) };
            }
            constexpr void execute_while_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_while_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance_i = container[index[0]];
                        auto& instance_j = container[index[1]];
                        return std::invoke(operation, std::forward<decltype(index)>(index), instance_i, instance_j);
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
            static constexpr void execute_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance_i, auto&& instance_j) { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& instance_i, auto&& instance_j) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance_i, auto&& instance_j) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& instance_i, auto&& instance_j) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance_i, auto&& instance_j) -> bool { ... }");
            }

		};

	};

}

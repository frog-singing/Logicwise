// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/index/sampler.h>
#include <logicwise/detail/vector_like_container_trait.h>
#include "traversal_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class elementwise_traversal;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::遍历 mode::traversal================================================================================

	template<typename Mode, typename Arrangement>
	class elementwise_traversal
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

        template<typename WrapperInstance>
        struct in_wrapper
        {
            static constexpr void execute(auto&&) noexcept {}
            static constexpr void execute_until(auto&&) noexcept {}

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
            using ProbeTypeI = typename List::template element<ProbeIndex>;

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeTypeI > ();
            }
            static constexpr void execute(OperationType&& operation)
            {
				template_execute_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> {
                        operation.template operator() < typename List::template element<Index> > ();
				    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeTypeI > () };
            }
            static constexpr void execute_until(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
					([&] <auto Index> { return
                        operation.template operator() < typename List::template element<Index> > ();
				    });
            }

            template<typename OperationType>
            static constexpr void execute(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <typename TypeI>() { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <typename TypeI>() -> bool { ... }");
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
            static constexpr auto ProbeValueI = List::template element<ProbeIndex>;

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                std::forward<OperationType>(operation)
                    .template operator() < ProbeValueI > ();
            }
            static constexpr void execute(OperationType&& operation)
            {
                template_execute_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> {
                        operation.template operator() < List::template element<Index> > ();
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation)
            {
                bool{ std::forward<OperationType>(operation)
                    .template operator() < ProbeValueI > () };
            }
            static constexpr void execute_until(OperationType&& operation)
            {
                template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
                    ([&] <auto Index> { return
                        operation.template operator() < List::template element<Index> > ();
                    });
            }

            template<typename OperationType>
            static constexpr void execute(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto ValueI>() { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] <auto ValueI>() -> bool { ... }");
            }

        };


        template<typename ContainerType>
        struct in_container
        {
            using ContainerTrait = vector_like_container_trait<ContainerType>;

            using StoredInstanceType = ContainerTrait::stored_instance_type;
            using StoredContainerType = ContainerTrait::stored_container_type;
            using ExpectedContainerType = ContainerTrait::expected_container_type;

            const StoredContainerType container;

            in_container() = delete; //禁用默认构造函数

            explicit constexpr in_container(ExpectedContainerType container) noexcept
                : container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

            template<typename OperationType>
                requires requires(OperationType&& operation, const StoredInstanceType& instance_i)
            {
                std::invoke(std::forward<OperationType>(operation), instance_i);
            }
            constexpr void execute(OperationType&& operation)
            {
                typename Arrangement::extent_type extent{ std::ranges::size(container) };

                instance_execute_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto index) {
                        std::invoke(operation, container[index]);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, const StoredInstanceType& instance_i)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), instance_i) };
            }
            constexpr void execute_until(OperationType&& operation)
            {
                typename Arrangement::extent_type extent{ std::ranges::size(container) };

                instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto index) { return
                        std::invoke(operation, container[index]);
                    });
            }

            template<typename OperationType>
            static constexpr void execute(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto instance_i) { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto instance_i) -> bool { ... }");
            }

        };

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/vector_like_container.h>

#include <logicwise/semantics/material/elementwise/type_list/traversal_protocol.h>
#include <logicwise/semantics/material/elementwise/value_list/traversal_protocol.h>
#include <logicwise/semantics/material/elementwise/vector_like_container/traversal_protocol.h>
#include "workshop/concrete_template_traversal.h"
#include "workshop/instance_traversal.h"


#include "executor/traversal_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename ModeOption, typename Arrangement>
	class elementwise_traversal;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::遍历 mode::traversal================================================================================

	template<typename ModeOption, typename Arrangement>
	class elementwise_traversal
	{
    private:
        using IndexTraverserType = typename ModeOption::template index_traverser<Arrangement>;

    public:
        template<typewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_type_list<WrapperInstance>;

            return typename concrete_template_traversal<
                type_list_traversal_protocol<List>,
                Arrangement,
                IndexTraverserType
            >::traversal{};
        }

        template<valuewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_value_list<WrapperInstance>;

            return typename concrete_template_traversal<
                value_list_traversal_protocol<List>,
                Arrangement,
                IndexTraverserType
            >::traversal{};
        }

        template<VectorLike ContainerType>
        [[nodiscard]] static constexpr auto in(ContainerType&& container)
        {
            return in_container<ContainerType>{ std::forward<ContainerType>(container) };
        }

        // traversal 具有 [[nodiscard]]
        template<VectorLike ContainerType>
        static constexpr auto in_refactoring(ContainerType&& container)
        {
            return typename instance_traversal<
                vector_like_container_traversal_protocol<ContainerType>,
                Arrangement,
                IndexTraverserType
            >::traversal{ std::forward<ContainerType>(container) };
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
        using ActualIndexType = decltype(std::declval<IndexTraverserType>().state());

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
                requires requires(OperationType&& operation, const StoredInstanceType& instance)
            {
                std::invoke(std::forward<OperationType>(operation), instance);
            }
            constexpr void execute(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        std::invoke(operation, container[index]);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index, const StoredInstanceType& instance)
            {
                std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index), instance);
            }
            constexpr void execute_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance = container[index];
                        std::invoke(operation, std::forward<decltype(index)>(index), instance);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), instance) };
            }
            constexpr void execute_until(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) { return
                        std::invoke(operation, container[index]);
                    });
            }
            
            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index), instance) };
            }
            constexpr void execute_until_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance = container[index];
                        return std::invoke(operation, std::forward<decltype(index)>(index), instance);
                    });
            }

            template<typename OperationType>
                requires requires(OperationType&& operation, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), instance) };
            }
            constexpr void execute_while(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_while_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) { return
                        std::invoke(operation, container[index]);
                    });
            }
            
            template<typename OperationType>
                requires requires(OperationType&& operation, ActualIndexType index, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<OperationType>(operation), static_cast<ActualIndexType>(index), instance) };
            }
            constexpr void execute_while_with_index(OperationType&& operation) const
            {
                extent_type extent{ std::ranges::size(container) };

                instance_execute_while_loop<Arrangement, IndexTraverserType>(extent,
                    [&] (auto&& index) {
                        auto& instance = container[index];
                        return std::invoke(operation, std::forward<decltype(index)>(index), instance);
                    });
            }

            template<typename OperationType>
            static constexpr void execute(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& instance) { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance) { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_until_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");
            }

            template<typename OperationType>
            static constexpr void execute_while_with_index(OperationType&&)
            {
                static_assert(dependent_false_v<OperationType>,
                    "[logicwise] Error: Incompatible operation signature!\n"
                    "Expected: [] (auto&& index, auto&& instance) -> bool { ... }");
            }

        };

	};

}

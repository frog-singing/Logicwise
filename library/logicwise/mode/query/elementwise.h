// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/semantics/vector_like_container.h>

#include <logicwise/semantics/material/elementwise/type_list/query_protocol.h>
#include <logicwise/semantics/material/elementwise/value_list/query_protocol.h>
#include <logicwise/semantics/material/elementwise/vector_like_container/query_protocol.h>
#include "workshop/concrete_template_query.h"
#include "workshop/instance_query.h"


#include "executor/query_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <cstddef> //用于 std::size_t
#include <optional> //用于 std::optional
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename ModeOption, typename Arrangement>
	class elementwise_querying;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename ModeOption, typename Arrangement>
	class elementwise_querying
	{
    public:
        template<typewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_type_list<WrapperInstance>;

            return typename concrete_template_query<
                type_list_query_protocol<List>,
                Arrangement
            >::query{};
        }

        template<valuewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_value_list<WrapperInstance>;

            return typename concrete_template_query<
                value_list_query_protocol<List>,
                Arrangement
            >::query{};
        }

        template<VectorLike ContainerType>
        [[nodiscard]] static constexpr auto in(ContainerType&& container)
        {
            return in_container<ContainerType>{ std::forward<ContainerType>(container) };
        }

        // query 具有 [[nodiscard]]
        template<VectorLike ContainerType>
        static constexpr auto in_refactoring(ContainerType&& container)
        {
            return typename instance_query<
                vector_like_container_query_protocol<ContainerType>,
                Arrangement
            >::query{ std::forward<ContainerType>(container) };
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
        using index_type = typename Arrangement::index_type;

        template<typename ContainerType>
        struct in_container
        {
            using ContainerTrait = vector_like_container_trait<ContainerType>;

            using RawInstanceType       = typename ContainerTrait::raw_instance_type;
            using StoredInstanceType    = typename ContainerTrait::stored_instance_type;
            using StoredContainerType   = typename ContainerTrait::stored_container_type;
            using ExpectedContainerType = typename ContainerTrait::expected_container_type;

            const StoredContainerType container;

            in_container() = delete; //禁用默认构造函数

            explicit constexpr in_container(ExpectedContainerType container) noexcept
                : container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

            template<typename PredicateType>
                requires requires(PredicateType&& predicate, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<PredicateType>(predicate), instance) };
            }
            [[nodiscard]] constexpr std::size_t count(PredicateType&& predicate) const
            {
                extent_type extent{ std::ranges::size(container) };

                return instance_count_loop<Arrangement>(extent,
				    [&] (auto&& index) { return
                        std::invoke(predicate, container[index]);
				    });
            }

            template<typename PredicateType>
                requires requires(PredicateType&& predicate, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<PredicateType>(predicate), instance) };
            }
            [[nodiscard]] constexpr std::optional<index_type> find_first_index(PredicateType&& predicate) const
            {
                extent_type extent{ std::ranges::size(container) };

                return instance_find_first_index_loop<Arrangement>(extent,
                    [&] (auto&& index) { return
                        std::invoke(predicate, container[index]);
                    });
            }

            template<typename PredicateType>
                requires requires(PredicateType&& predicate, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<PredicateType>(predicate), instance) };
            }
            [[nodiscard]] constexpr std::optional<index_type> find_last_index(PredicateType&& predicate) const
            {
                extent_type extent{ std::ranges::size(container) };

                return instance_find_last_index_loop<Arrangement>(extent,
                    [&] (auto&& index) { return
                        std::invoke(predicate, container[index]);
                    });
            }

            template<typename PredicateType>
                requires requires(PredicateType&& predicate, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<PredicateType>(predicate), instance) };
            }
            [[nodiscard]] constexpr auto find_first(PredicateType&& predicate) const
            {
                extent_type extent{ std::ranges::size(container) };

                auto found_index = instance_find_first_index_loop<Arrangement>(extent,
				    [&] (auto&& index) { return
                        std::invoke(predicate, container[index]);
				    });

#if defined(__cpp_lib_optional) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_23
                //C++23
                return found_index.transform([&] (auto&& index) -> RawInstanceType {
                        return container[index];
                    });
#else
                //C++20
                std::optional<RawInstanceType> found_instance
                {
                    !found_index ? std::nullopt :
                    std::make_optional(container[*found_index])
                };

                return found_instance;
#endif
            }
                        
            template<typename PredicateType>
                requires requires(PredicateType&& predicate, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<PredicateType>(predicate), instance) };
            }
            [[nodiscard]] constexpr auto find_last(PredicateType&& predicate) const
            {
                extent_type extent{ std::ranges::size(container) };

                auto found_index = instance_find_last_index_loop<Arrangement>(extent,
				    [&] (auto&& index) { return
                        std::invoke(predicate, container[index]);
				    });

#if defined(__cpp_lib_optional) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_23
                //C++23
                return found_index.transform([&] (auto&& index) -> RawInstanceType {
                        return container[index];
                    });
#else
                //C++20
                std::optional<RawInstanceType> found_instance
                {
                    !found_index ? std::nullopt :
                    std::make_optional(container[*found_index])
                };

                return found_instance;
#endif
            }

            template<typename PredicateType>
            static constexpr std::size_t count(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return 0;
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_first_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_last_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr auto find_first(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return std::optional<RawInstanceType>{};
            }

            template<typename PredicateType>
            static constexpr auto find_last(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return std::optional<RawInstanceType>{};
            }

        };

	};

}

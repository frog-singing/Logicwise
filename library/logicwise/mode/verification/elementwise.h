// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/vector_like_container.h>

#include <logicwise/semantics/material/elementwise/type_list/verification_protocol.h>
#include <logicwise/semantics/material/elementwise/value_list/verification_protocol.h>
#include <logicwise/semantics/material/elementwise/vector_like_container/verification_protocol.h>
#include "workshop/template_verification_workshop.h"
#include "workshop/instance_verification_workshop.h"


#include "executor/verification_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
    template<typename Quantifier, typename Arrangement>
    class elementwise_verification;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::验证 mode::verification================================================================================

    template<typename Quantifier, typename Arrangement>
	class elementwise_verification
	{
    public:
        template<typewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_type_list<WrapperInstance>;

            return typename template_verification_workshop<
                type_list_verification_protocol<List>,
                Quantifier,
                Arrangement
            >::verification{};
        }

        template<valuewise::List WrapperInstance>
        static constexpr auto in()
        {
            using List = as_value_list<WrapperInstance>;

            return typename template_verification_workshop<
                value_list_verification_protocol<List>,
                Quantifier,
                Arrangement
            >::verification{};
        }

        template<VectorLike ContainerType>
        [[nodiscard]] static constexpr auto in(ContainerType&& container)
        {
            return in_container<ContainerType>{ std::forward<ContainerType>(container) };
        }

        // verification 具有 [[nodiscard]]
        template<VectorLike ContainerType>
        static constexpr auto in_refactoring(ContainerType&& container)
        {
            return typename instance_verification_workshop<
                vector_like_container_verification_protocol<ContainerType>,
                Quantifier,
                Arrangement
            >::verification{ std::forward<ContainerType>(container) };
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

            template<typename VerifierType>
                requires requires(VerifierType&& verifier, const StoredInstanceType& instance)
            {
                bool{ std::invoke(std::forward<VerifierType>(verifier), instance) };
            }
            [[nodiscard]] constexpr bool satisfies(VerifierType&& verifier) const
            {
                extent_type extent{ std::ranges::size(container) };

			    return instance_satisfies_loop<Quantifier, Arrangement>(extent,
				    [&] (auto&& index) { return
                        std::invoke(verifier, container[index]);
				    });
            }

            template<typename VerifierType>
            static constexpr bool satisfies(VerifierType&&)
            {
                static_assert(dependent_false_v<VerifierType>,
                    "[logicwise] Error: Incompatible verifier signature!\n"
                    "Expected: [] (auto&& instance) -> bool { ... }");

                return false;
            }

        };

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "template_validation_loop.h"

#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/index/sampler.h>
#include <logicwise/index/index_sequencer.h>
#include <logicwise/detail/vector_like_container_trait.h>
#include <ranges> //用于 std::ranges，C++20标准
#include <functional> //用于 std::invoke
#include <utility> //用于 std::forward


namespace logicwise::detail
{
    struct elementwise_validator;

    template<typename Quantifier, typename Arrangement>
    class elementwise_validation;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

    struct elementwise_validator
    {
        template<typename Quantifier, typename Arrangement, typename TypeList,
            template<typename> typename Validator, typename TraitCertificate>
        static constexpr bool validate_type_list_with_trait()
        {
            if constexpr (requires { bool{ TraitCertificate::value }; })
            {
                return validate_type_list_with_invocable<Quantifier, Arrangement, TypeList>
                    ([] <typename TypeI> { return Validator<TypeI>::value; });
            }
            else if constexpr (requires { bool{ TraitCertificate{} }; })
            {
                return validate_type_list_with_invocable<Quantifier, Arrangement, TypeList>
                    ([] <typename TypeI> { return Validator<TypeI>{}; });
            }
            else
            {
                static_assert(dependent_false_v<TraitCertificate>,
                    "[logicwise] Error: Unsupported trait validator!\n"
                    "Certificate must provide '::value' or be convertible to bool via construction.");

                return false;
            }
        }

        template<typename Quantifier, typename Arrangement, typename TypeList,
            typename ValidatorType>
        static constexpr bool validate_type_list_with_invocable(ValidatorType&& validator)
        {
            LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				{ TypeList::size },
				(
                    validator_ref.template operator() < typename TypeList::template element<Index> > ()
				)
			);
        }

        template<typename Quantifier, typename Arrangement, typename ValueList,
            template<auto> typename Validator, typename TraitCertificate>
        static constexpr bool validate_value_list_with_trait()
        {
            if constexpr (requires { bool{ TraitCertificate::value }; })
            {
                return validate_value_list_with_invocable<Quantifier, Arrangement, ValueList>
                    ([] <auto ValueI> { return Validator<ValueI>::value; });
            }
            else if constexpr (requires { bool{ TraitCertificate{} }; })
            {
                return validate_value_list_with_invocable<Quantifier, Arrangement, ValueList>
                    ([] <auto ValueI> { return Validator<ValueI>{}; });
            }
            else
            {
                static_assert(dependent_false_v<TraitCertificate>,
                    "[logicwise] Error: Unsupported trait validator!\n"
                    "Certificate must provide '::value' or be convertible to bool via construction.");

                return false;
            }
        }

        template<typename Quantifier, typename Arrangement, typename ValueList,
            typename ValidatorType>
        static constexpr bool validate_value_list_with_invocable(ValidatorType&& validator)
        {
            LOGICWISE_TEMPLATE_VALIDATION_LOOP
            (
                { ValueList::size },
                (
                    validator_ref.template operator() < ValueList::template element<Index> > ()
                )
            );
        }

        template<typename Quantifier, typename Arrangement,
            typename ContainerType, typename ValidatorType>
        static constexpr bool validate_container(const ContainerType& container, ValidatorType&& validator)
        {
            using extent_type = typename Arrangement::extent_type;
            using index_traverser_type = typename Arrangement::fast_index_traverser;

            typename Quantifier::solver quantifier_solver{};

            extent_type extent{ std::ranges::size(container) };
            index_traverser_type index_traverser{ extent };

            //遍历入口处将 validator 固定为左值引用，避免不必要的复制
            auto& validator_ref = validator;

            while (!quantifier_solver.solved() && !index_traverser.done())
            {
                auto i = index_traverser.state();

                quantifier_solver.step
                (
                    std::invoke(validator_ref, container[i])
                );

                index_traverser.step();
            }

            return quantifier_solver.result();
        }

    };

    //================================================================================

	template<typename Quantifier, typename Arrangement>
	class elementwise_validation
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

        template<typename WrapperInstance>
        struct in_wrapper
        {
            static constexpr bool validation_result{ typename Quantifier::solver{}.result() };

            template<template<typename> typename>
            [[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

            [[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return validation_result; }

        };

        template<typewise::List TypeList>
            requires TraversableExtent <
                Arrangement,
                extent_type{ as_type_list<TypeList>::size }
            >
        struct in_wrapper<TypeList>
        {
            using List = as_type_list<TypeList>;

            static constexpr auto ProbeIndex = probe_index <
                Arrangement,
                extent_type{ List::size }
            > ;

            using ProbeTypeI = typename List::template element<ProbeIndex>;

            template<template<typename> typename Validator>
                requires requires { typename Validator<ProbeTypeI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Validator<ProbeTypeI>;

                return elementwise_validator::template validate_type_list_with_trait
                    <Quantifier, Arrangement, List, Validator, TraitCertificate>();
            }

            template<typename ValidatorType>
                requires requires(ValidatorType&& validator)
            {
                bool{ std::forward<ValidatorType>(validator)
                    .template operator() < ProbeTypeI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
            {
                return elementwise_validator::template validate_type_list_with_invocable
                    <Quantifier, Arrangement, List>
                    (std::forward<ValidatorType>(validator));
            }

            template<typename ValidatorType>
            static constexpr bool satisfies(ValidatorType&&)
            {
                static_assert(dependent_false_v<ValidatorType>,
                    "[logicwise] Error: Incompatible validator signature!\n"
                    "Expected: [] <typename TypeI>() -> bool { ... }");

                return false;
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

            static constexpr auto ProbeIndex = probe_index <
                Arrangement,
                extent_type{ List::size }
            > ;

            static constexpr auto ProbeValueI = List::template element<ProbeIndex>;

            template<template<auto> typename Validator>
                requires requires { typename Validator<ProbeValueI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Validator<ProbeValueI>;

                return elementwise_validator::template validate_value_list_with_trait
                    <Quantifier, Arrangement, List, Validator, TraitCertificate>();
            }

            template<typename ValidatorType>
				requires requires(ValidatorType && validator)
            {
                bool{ std::forward<ValidatorType>(validator)
                    .template operator() < ProbeValueI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
            {
                return elementwise_validator::template validate_value_list_with_invocable
                    <Quantifier, Arrangement, List>
                    (std::forward<ValidatorType>(validator));
            }

            template<typename ValidatorType>
            static constexpr bool satisfies(ValidatorType&&)
            {
                static_assert(dependent_false_v<ValidatorType>,
                    "[logicwise] Error: Incompatible validator signature!\n"
                    "Expected: [] <auto ValueI>() -> bool { ... }");

                return false;
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

            template<typename ValidatorType>
                requires requires(ValidatorType&& validator, const StoredInstanceType& instance_i)
            {
                bool{ std::invoke(std::forward<ValidatorType>(validator), instance_i) };
            }
            [[nodiscard]] constexpr bool satisfies(ValidatorType&& validator) const
            {
                return elementwise_validator::template validate_container
                    <Quantifier, Arrangement>
                    (container, std::forward<ValidatorType>(validator));
            }

            template<typename ValidatorType>
            static constexpr bool satisfies(ValidatorType&&)
            {
                static_assert(dependent_false_v<ValidatorType>,
                    "[logicwise] Error: Incompatible validator signature!\n"
                    "Expected: [] (auto instance_i) -> bool { ... }");

                return false;
            }

        };

	};

}

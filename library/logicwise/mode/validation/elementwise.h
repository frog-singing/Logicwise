// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/vector_like_container.h>
#include "validation_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
    template<typename Quantifier, typename Arrangement>
    class elementwise_validation;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

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
            static constexpr extent_type Extent{ List::size };
            static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
            using ProbeTypeI = typename List::template element<ProbeIndex>;

            template<typename ValidatorType>
                requires requires(ValidatorType&& validator)
            {
                bool{ std::forward<ValidatorType>(validator)
                    .template operator() < ProbeTypeI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
            {
                return template_validation_loop<Quantifier, Arrangement, Extent>
				    ([&] <auto Index> { return
                        validator.template operator() < typename List::template element<Index> > ();
				    });
            }

            template<template<typename> typename Validator>
                requires requires { typename Validator<ProbeTypeI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Validator<ProbeTypeI>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_validation_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Validator< typename List::template element<Index> > > ();
                    });
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
            static constexpr extent_type Extent{ List::size };
            static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
            static constexpr auto ProbeValueI = List::template element<ProbeIndex>;

            template<typename ValidatorType>
				requires requires(ValidatorType && validator)
            {
                bool{ std::forward<ValidatorType>(validator)
                    .template operator() < ProbeValueI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
            {
                return template_validation_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        validator.template operator() < List::template element<Index> > ();
                    });
            }

            template<template<auto> typename Validator>
                requires requires { typename Validator<ProbeValueI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Validator<ProbeValueI>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_validation_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Validator< List::template element<Index> > > ();
                    });
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
                extent_type extent{ std::ranges::size(container) };

			    return instance_validation_loop<Quantifier, Arrangement>(extent,
				    [&] (auto&& index) { return
                        std::invoke(validator, container[index]);
				    });
            }

            template<typename ValidatorType>
            static constexpr bool satisfies(ValidatorType&&)
            {
                static_assert(dependent_false_v<ValidatorType>,
                    "[logicwise] Error: Incompatible validator signature!\n"
                    "Expected: [] (auto&& instance_i) -> bool { ... }");

                return false;
            }

        };

	};

}

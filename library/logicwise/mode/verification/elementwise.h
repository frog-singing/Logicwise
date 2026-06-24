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
#include "verification_loop.h"
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
	//行为模式::验证 mode::verification================================================================================

    template<typename Quantifier, typename Arrangement>
	class elementwise_verification
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
            static constexpr bool verification_result{ typename Quantifier::solver{}.result() };

            [[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return verification_result; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr bool satisfies() noexcept { return verification_result; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr bool satisfies() noexcept { return verification_result; }

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

            template<typename VerifierType>
                requires requires(VerifierType&& verifier)
            {
                bool{ std::forward<VerifierType>(verifier)
                    .template operator() < ProbeTypeI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(VerifierType&& verifier)
            {
                return template_verification_loop<Quantifier, Arrangement, Extent>
				    ([&] <auto Index> { return
                        verifier.template operator() < typename List::template element<Index> > ();
				    });
            }

            template<template<typename> typename Verifier>
                requires requires { typename Verifier<ProbeTypeI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Verifier<ProbeTypeI>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_verification_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Verifier< typename List::template element<Index> > > ();
                    });
            }

            template<typename VerifierType>
            static constexpr bool satisfies(VerifierType&&)
            {
                static_assert(dependent_false_v<VerifierType>,
                    "[logicwise] Error: Incompatible verifier signature!\n"
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

            template<typename VerifierType>
				requires requires(VerifierType && verifier)
            {
                bool{ std::forward<VerifierType>(verifier)
                    .template operator() < ProbeValueI > () };
            }
            [[nodiscard]] static constexpr bool satisfies(VerifierType&& verifier)
            {
                return template_verification_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        verifier.template operator() < List::template element<Index> > ();
                    });
            }

            template<template<auto> typename Verifier>
                requires requires { typename Verifier<ProbeValueI>; }
            [[nodiscard]] static constexpr bool satisfies()
            {
                using TraitCertificate = Verifier<ProbeValueI>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_verification_loop<Quantifier, Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Verifier< List::template element<Index> > > ();
                    });
            }

            template<typename VerifierType>
            static constexpr bool satisfies(VerifierType&&)
            {
                static_assert(dependent_false_v<VerifierType>,
                    "[logicwise] Error: Incompatible verifier signature!\n"
                    "Expected: [] <auto ValueI>() -> bool { ... }");

                return false;
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

            template<typename VerifierType>
                requires requires(VerifierType&& verifier, const StoredInstanceType& instance_i)
            {
                bool{ std::invoke(std::forward<VerifierType>(verifier), instance_i) };
            }
            [[nodiscard]] constexpr bool satisfies(VerifierType&& verifier) const
            {
                extent_type extent{ std::ranges::size(container) };

			    return instance_verification_loop<Quantifier, Arrangement>(extent,
				    [&] (const auto& index) { return
                        std::invoke(verifier, container[index]);
				    });
            }

            template<typename VerifierType>
            static constexpr bool satisfies(VerifierType&&)
            {
                static_assert(dependent_false_v<VerifierType>,
                    "[logicwise] Error: Incompatible verifier signature!\n"
                    "Expected: [] (auto&& instance_i) -> bool { ... }");

                return false;
            }

        };

	};

}

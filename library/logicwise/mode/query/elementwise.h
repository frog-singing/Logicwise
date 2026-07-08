// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/element.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/index/sampler.h>
#include <logicwise/semantics/trait_predicate.h>
#include <logicwise/semantics/vector_like_container.h>
#include "query_loop.h"
#include <utility> //用于 std::forward
#include <functional> //用于 std::invoke
#include <cstddef> //用于 std::size_t
#include <optional> //用于 std::optional
#include <variant> //用于 std::monostate
#include <ranges> //用于 std::ranges，C++20标准


namespace logicwise::detail
{
	template<typename Mode, typename Arrangement>
	class elementwise_querying;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::查询 mode::query================================================================================

	template<typename Mode, typename Arrangement>
	class elementwise_querying
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
        using index_type = typename Arrangement::index_type;

        template<typename WrapperInstance>
        struct in_wrapper
        {
            [[nodiscard]] static constexpr std::size_t count(auto&&) noexcept { return 0; }
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index(auto&&) noexcept { return {}; }
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index(auto&&) noexcept { return {}; }

            [[nodiscard]] static constexpr auto find_first(auto&&) noexcept
                requires typewise::List<WrapperInstance>
            { return type_element<void>{}; }

            [[nodiscard]] static constexpr auto find_last(auto&&) noexcept
                requires typewise::List<WrapperInstance>
            { return type_element<void>{}; }

            [[nodiscard]] static constexpr auto find_first(auto&&) noexcept
                requires valuewise::List<WrapperInstance>
            { return std::monostate{}; }

            [[nodiscard]] static constexpr auto find_last(auto&&) noexcept
                requires valuewise::List<WrapperInstance>
            { return std::monostate{}; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr std::size_t count() noexcept { return 0; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index() noexcept { return {}; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index() noexcept { return {}; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr auto find_first() noexcept { return type_element<void>{}; }

            template<template<typename> typename>
            [[nodiscard]] static constexpr auto find_last() noexcept { return type_element<void>{}; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr std::size_t count() noexcept { return 0; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index() noexcept { return {}; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index() noexcept { return {}; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr auto find_first() noexcept { return std::monostate{}; }

            template<template<auto> typename>
            [[nodiscard]] static constexpr auto find_last() noexcept { return std::monostate{}; }

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
            using ProbeType = typename List::template element<ProbeIndex>;

            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeType > () };
            }
            [[nodiscard]] static constexpr std::size_t count(PredicateType&& predicate)
            {
                return template_count_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < typename List::template element<Index> > ();
				    });
            }
            
            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeType > () };
            }
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index(PredicateType&& predicate)
            {
                return template_find_first_index_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < typename List::template element<Index> > ();
				    });
            }
            
            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeType > () };
            }
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index(PredicateType&& predicate)
            {
                return template_find_last_index_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < typename List::template element<Index> > ();
				    });
            }

            template<typename Predicate>
                requires requires
            {
                bool{ Predicate{}.template operator() < ProbeType > () };
            }
            [[nodiscard]] static constexpr auto find_first(Predicate&&)
            {
                constexpr auto found_index = template_find_first_index_loop<Arrangement, Extent>
				    ([] <auto Index> { return
                        Predicate{}.template operator() < typename List::template element<Index> > ();
				    });

                if constexpr (found_index)
                {
                    return type_element< typename List::template element<*found_index> >{};
                }
                else { return type_element<void>{}; }
            }

            template<typename Predicate>
                requires requires
            {
                bool{ Predicate{}.template operator() < ProbeType > () };
            }
            [[nodiscard]] static constexpr auto find_last(Predicate&&)
            {
                constexpr auto found_index = template_find_last_index_loop<Arrangement, Extent>
				    ([] <auto Index> { return
                        Predicate{}.template operator() < typename List::template element<Index> > ();
				    });

                if constexpr (found_index)
                {
                    return type_element< typename List::template element<*found_index> >{};
                }
                else { return type_element<void>{}; }
            }

            template<template<typename> typename Predicate>
                requires requires { typename Predicate<ProbeType>; }
            [[nodiscard]] static constexpr std::size_t count()
            {
                using TraitCertificate = Predicate<ProbeType>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_count_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< typename List::template element<Index> > > ();
                    });
            }

            template<template<typename> typename Predicate>
                requires requires { typename Predicate<ProbeType>; }
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index()
            {
                using TraitCertificate = Predicate<ProbeType>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_find_first_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< typename List::template element<Index> > > ();
                    });
            }

            template<template<typename> typename Predicate>
                requires requires { typename Predicate<ProbeType>; }
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index()
            {
                using TraitCertificate = Predicate<ProbeType>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_find_last_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< typename List::template element<Index> > > ();
                    });
            }

            template<template<typename> typename Predicate>
                requires requires { typename Predicate<ProbeType>; }
            [[nodiscard]] static constexpr auto find_first()
            {
                using TraitCertificate = Predicate<ProbeType>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                constexpr auto found_index = template_find_first_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< typename List::template element<Index> > > ();
                    });

                if constexpr (found_index)
                {
                    return type_element< typename List::template element<*found_index> >{};
                }
                else { return type_element<void>{}; }
            }

            template<template<typename> typename Predicate>
                requires requires { typename Predicate<ProbeType>; }
            [[nodiscard]] static constexpr auto find_last()
            {
                using TraitCertificate = Predicate<ProbeType>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                constexpr auto found_index = template_find_last_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< typename List::template element<Index> > > ();
                    });

                if constexpr (found_index)
                {
                    return type_element< typename List::template element<*found_index> >{};
                }
                else { return type_element<void>{}; }
            }

            template<typename PredicateType>
            static constexpr std::size_t count(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <typename Type>() -> bool { ... }");

                return 0;
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_first_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <typename Type>() -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_last_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <typename Type>() -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr auto find_first(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

                return type_element<void>{};
            }

            template<typename PredicateType>
            static constexpr auto find_last(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected non-capturing lambda: [] <typename Type>() -> bool { ... }");

                return type_element<void>{};
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
            static constexpr auto ProbeValue = List::template element<ProbeIndex>;

            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeValue > () };
            }
            [[nodiscard]] static constexpr std::size_t count(PredicateType&& predicate)
            {
                return template_count_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < List::template element<Index> > ();
				    });
            }

            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeValue > () };
            }
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index(PredicateType&& predicate)
            {
                return template_find_first_index_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < List::template element<Index> > ();
				    });
            }

            template<typename PredicateType>
                requires requires(PredicateType&& predicate)
            {
                bool{ std::forward<PredicateType>(predicate)
                    .template operator() < ProbeValue > () };
            }
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index(PredicateType&& predicate)
            {
                return template_find_last_index_loop<Arrangement, Extent>
				    ([&] <auto Index> { return
                        predicate.template operator() < List::template element<Index> > ();
				    });
            }

            template<typename Predicate>
                requires requires
            {
                bool{ Predicate{}.template operator() < ProbeValue > () };
            }
            [[nodiscard]] static constexpr auto find_first(Predicate&&)
            {
                constexpr auto found_index = template_find_first_index_loop<Arrangement, Extent>
				    ([] <auto Index> { return
                        Predicate{}.template operator() < List::template element<Index> > ();
				    });

                if constexpr (found_index)
                {
                    return List::template element<*found_index>;
                }
                else { return std::monostate{}; }
            }

            template<typename Predicate>
                requires requires
            {
                bool{ Predicate{}.template operator() < ProbeValue > () };
            }
            [[nodiscard]] static constexpr auto find_last(Predicate&&)
            {
                constexpr auto found_index = template_find_last_index_loop<Arrangement, Extent>
				    ([] <auto Index> { return
                        Predicate{}.template operator() < List::template element<Index> > ();
				    });

                if constexpr (found_index)
                {
                    return List::template element<*found_index>;
                }
                else { return std::monostate{}; }
            }

            template<template<auto> typename Predicate>
                requires requires { typename Predicate<ProbeValue>; }
            [[nodiscard]] static constexpr std::size_t count()
            {
                using TraitCertificate = Predicate<ProbeValue>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_count_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< List::template element<Index> > > ();
                    });
            }
                        
            template<template<auto> typename Predicate>
                requires requires { typename Predicate<ProbeValue>; }
            [[nodiscard]] static constexpr std::optional<index_type> find_first_index()
            {
                using TraitCertificate = Predicate<ProbeValue>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_find_first_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< List::template element<Index> > > ();
                    });
            }

            template<template<auto> typename Predicate>
                requires requires { typename Predicate<ProbeValue>; }
            [[nodiscard]] static constexpr std::optional<index_type> find_last_index()
            {
                using TraitCertificate = Predicate<ProbeValue>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                return template_find_last_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< List::template element<Index> > > ();
                    });
            }

            template<template<auto> typename Predicate>
                requires requires { typename Predicate<ProbeValue>; }
            [[nodiscard]] static constexpr auto find_first()
            {
                using TraitCertificate = Predicate<ProbeValue>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                constexpr auto found_index = template_find_first_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< List::template element<Index> > > ();
                    });

                if constexpr (found_index)
                {
                    return List::template element<*found_index>;
                }
                else { return std::monostate{}; }
            }

            template<template<auto> typename Predicate>
                requires requires { typename Predicate<ProbeValue>; }
            [[nodiscard]] static constexpr auto find_last()
            {
                using TraitCertificate = Predicate<ProbeValue>;
                constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

                constexpr auto found_index = template_find_last_index_loop<Arrangement, Extent>
                    ([&] <auto Index> { return
                        PredicateSolver.template operator() < Predicate< List::template element<Index> > > ();
                    });

                if constexpr (found_index)
                {
                    return List::template element<*found_index>;
                }
                else { return std::monostate{}; }
            }

            template<typename PredicateType>
            static constexpr std::size_t count(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <auto Value>() -> bool { ... }");

                return 0;
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_first_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <auto Value>() -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr std::optional<index_type> find_last_index(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected: [] <auto Value>() -> bool { ... }");

                return {};
            }

            template<typename PredicateType>
            static constexpr auto find_first(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected non-capturing lambda: [] <auto Value>() -> bool { ... }");

                return std::monostate{};
            }

            template<typename PredicateType>
            static constexpr auto find_last(PredicateType&&)
            {
                static_assert(dependent_false_v<PredicateType>,
                    "[logicwise] Error: Incompatible predicate signature!\n"
                    "Expected non-capturing lambda: [] <auto Value>() -> bool { ... }");

                return std::monostate{};
            }

        };


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

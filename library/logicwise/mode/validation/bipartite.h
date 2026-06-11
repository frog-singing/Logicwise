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
	class bipartite_validation;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

	template<typename Quantifier, typename Arrangement>
	class bipartite_validation
	{
	public:
		template<typename WrapperInstanceA, typename WrapperInstanceB>
			requires (typewise::List<WrapperInstanceA> || valuewise::List<WrapperInstanceA>) &&
		(typewise::List<WrapperInstanceB> || valuewise::List<WrapperInstanceB>)
		[[nodiscard]] static constexpr auto between()
		{
			return between_wrapper<WrapperInstanceA, WrapperInstanceB>{};
		}

		template<VectorLike ContainerTypeA, VectorLike ContainerTypeB>
		[[nodiscard]] static constexpr auto between(ContainerTypeA&& containerA, ContainerTypeB&& containerB)
		{
			return between_container<ContainerTypeA, ContainerTypeB>
			{ std::forward<ContainerTypeA>(containerA), std::forward<ContainerTypeB>(containerB) };
		}

		template<typename WrapperInstance, VectorLike ContainerType>
			requires typewise::List<WrapperInstance> || valuewise::List<WrapperInstance>
		[[nodiscard]] static constexpr auto between(ContainerType&& container)
		{
			return between_wrapper_and_container<WrapperInstance, ContainerType>
			{ std::forward<ContainerType>(container) };
		}

		template<typename WrapperInstanceA, typename WrapperInstanceB>
		static constexpr auto between()
		{
			static_assert(dependent_false_v<WrapperInstanceA, WrapperInstanceB>,
				"[logicwise] Error: Incompatible <WrapperInstance>!\n"
				"Expected: pure type list or pure value list.");

			return false;
		}

		template<typename ContainerTypeA, typename ContainerTypeB>
		static constexpr auto between(ContainerTypeA&&, ContainerTypeB&&)
		{
			static_assert(dependent_false_v<ContainerTypeA, ContainerTypeB>,
				"[logicwise] Error: Incompatible container!\n"
				"Expected: VectorLike container.");

			return false;
		}

		template<typename WrapperInstance, typename ContainerType>
		static constexpr auto between(ContainerType&&)
		{
			static_assert(dependent_false_v<WrapperInstance, ContainerType>,
				"[logicwise] Error: Incompatible <WrapperInstance> or container!\n"
				"Expected <WrapperInstance>: pure type list or pure value list.\n"
				"Expected container: VectorLike container.");

			return false;
		}


	private:
		using extent_type = typename Arrangement::extent_type;

		template<typename WrapperInstanceA, typename WrapperInstanceB>
		struct between_wrapper
		{
			static constexpr bool validation_result{ typename Quantifier::solver{}.result() };

			template<template<typename, typename> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

			template<template<auto, auto> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

			template<template<typename, auto> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

			template<template<auto, typename> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

			[[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return validation_result; }

		};

		template<typewise::List TypeListA, typewise::List TypeListB>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_type_list<TypeListA>::size, as_type_list<TypeListB>::size }
			>
		struct between_wrapper<TypeListA, TypeListB>
		{
			using ListA = as_type_list<TypeListA>;
			using ListB = as_type_list<TypeListB>;
			static constexpr extent_type Extent{ ListA::size, ListB::size };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			using ProbeTypeI = typename ListA::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename ListB::template element<ProbeIndex[1]>;

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI, ProbeTypeJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator() <
							typename ListA::template element<Index[0]>,
							typename ListB::template element<Index[1]>
						> ();
					});
			}

			template<template<typename, typename> typename Validator>
				requires requires { typename Validator<ProbeTypeI, ProbeTypeJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeTypeI, ProbeTypeJ>;
				constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						PredicateSolver.template operator() < Validator<
							typename ListA::template element<Index[0]>,
							typename ListB::template element<Index[1]>
						> > ();
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <typename TypeI, typename TypeJ>() -> bool { ... }");

				return false;
			}

		};

		template<valuewise::List ValueListA, valuewise::List ValueListB>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_value_list<ValueListA>::size, as_value_list<ValueListB>::size }
			>
		struct between_wrapper<ValueListA, ValueListB>
		{
			using ListA = as_value_list<ValueListA>;
			using ListB = as_value_list<ValueListB>;
			static constexpr extent_type Extent{ ListA::size, ListB::size };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			static constexpr auto ProbeValueI = ListA::template element<ProbeIndex[0]>;
			static constexpr auto ProbeValueJ = ListB::template element<ProbeIndex[1]>;

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI, ProbeValueJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator() <
							ListA::template element<Index[0]>,
							ListB::template element<Index[1]>
						> ();
					});
			}

			template<template<auto, auto> typename Validator>
				requires requires { typename Validator<ProbeValueI, ProbeValueJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeValueI, ProbeValueJ>;
				constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						PredicateSolver.template operator() < Validator<
							ListA::template element<Index[0]>,
							ListB::template element<Index[1]>
						> > ();
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <auto ValueI, auto ValueJ>() -> bool { ... }");

				return false;
			}

		};

		template<typewise::List TypeList, valuewise::List ValueList>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_type_list<TypeList>::size, as_value_list<ValueList>::size }
			>
		struct between_wrapper<TypeList, ValueList>
		{
			using ListA = as_type_list<TypeList>;
			using ListB = as_value_list<ValueList>;
			static constexpr extent_type Extent{ ListA::size, ListB::size };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			using ProbeTypeI = typename ListA::template element<ProbeIndex[0]>;
			static constexpr auto ProbeValueJ = ListB::template element<ProbeIndex[1]>;

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI, ProbeValueJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator() <
							typename ListA::template element<Index[0]>,
							ListB::template element<Index[1]>
						> ();
					});
			}

			template<template<typename, auto> typename Validator>
				requires requires { typename Validator<ProbeTypeI, ProbeValueJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeTypeI, ProbeValueJ>;
				constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						PredicateSolver.template operator() < Validator<
							typename ListA::template element<Index[0]>,
							ListB::template element<Index[1]>
						> > ();
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <typename TypeI, auto ValueJ>() -> bool { ... }");

				return false;
			}

		};

		template<valuewise::List ValueList, typewise::List TypeList>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_value_list<ValueList>::size, as_type_list<TypeList>::size }
			>
		struct between_wrapper<ValueList, TypeList>
		{
			using ListA = as_value_list<ValueList>;
			using ListB = as_type_list<TypeList>;
			static constexpr extent_type Extent{ ListA::size, ListB::size };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			static constexpr auto ProbeValueI = ListA::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename ListB::template element<ProbeIndex[1]>;

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI, ProbeTypeJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator() <
							ListA::template element<Index[0]>,
							typename ListB::template element<Index[1]>
						> ();
					});
			}

			template<template<auto, typename> typename Validator>
				requires requires { typename Validator<ProbeValueI, ProbeTypeJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeValueI, ProbeTypeJ>;
				constexpr auto PredicateSolver{ trait_predicate_solver<TraitCertificate> };

				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						PredicateSolver.template operator() < Validator<
							ListA::template element<Index[0]>,
							typename ListB::template element<Index[1]>
						> > ();
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <auto ValueI, typename TypeJ>() -> bool { ... }");

				return false;
			}

		};

		//--------------------------------------------------------------------------------

		template<typename ContainerTypeA, typename ContainerTypeB>
		struct between_container
		{
			using ContainerTraitA = vector_like_container_trait<ContainerTypeA>;

			using StoredInstanceTypeA = ContainerTraitA::stored_instance_type;
			using StoredContainerTypeA = ContainerTraitA::stored_container_type;
			using ExpectedContainerTypeA = ContainerTraitA::expected_container_type;

			using ContainerTraitB = vector_like_container_trait<ContainerTypeB>;

			using StoredInstanceTypeB = ContainerTraitB::stored_instance_type;
			using StoredContainerTypeB = ContainerTraitB::stored_container_type;
			using ExpectedContainerTypeB = ContainerTraitB::expected_container_type;

			const StoredContainerTypeA containerA;
			const StoredContainerTypeB containerB;

			between_container() = delete; //禁用默认构造函数

			explicit constexpr between_container(
				ExpectedContainerTypeA containerA, ExpectedContainerTypeB containerB) noexcept
				: containerA{ ContainerTraitA::cast_container(static_cast<ExpectedContainerTypeA>(containerA)) }
				, containerB{ ContainerTraitB::cast_container(static_cast<ExpectedContainerTypeB>(containerB)) }
			{}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator,
			const StoredInstanceTypeA& instance_i, const StoredInstanceTypeB& instance_j)
			{
				bool{ std::invoke(std::forward<ValidatorType>(validator), instance_i, instance_j) };
			}
			[[nodiscard]] constexpr bool satisfies(ValidatorType&& validator) const
			{
				extent_type extent{ std::ranges::size(containerA), std::ranges::size(containerB) };

				return instance_validation_loop<Quantifier, Arrangement>(extent,
					[&] (auto&& index) { return
						std::invoke(validator, containerA[index[0]], containerB[index[1]]);
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] (auto&& instance_i, auto&& instance_j) -> bool { ... }");

				return false;
			}

		};

		//--------------------------------------------------------------------------------

		template<typename WrapperInstance, typename ContainerType>
		struct between_wrapper_and_container
		{
			explicit constexpr between_wrapper_and_container(auto&&) noexcept {}

			static constexpr bool validation_result{ typename Quantifier::solver{}.result() };

			[[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return validation_result; }

		};

		template<typewise::List TypeList, typename ContainerType>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_type_list<TypeList>::size, static_container_size<ContainerType> }
			>
		struct between_wrapper_and_container<TypeList, ContainerType>
		{
			using List = as_type_list<TypeList>;
			static constexpr auto ContainerSize = static_container_size<ContainerType>;
			static constexpr extent_type Extent{ List::size, ContainerSize };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			using ProbeTypeI = typename List::template element<ProbeIndex[0]>;

			using ContainerTrait = vector_like_container_trait<ContainerType>;

			using StoredInstanceType = ContainerTrait::stored_instance_type;
			using StoredContainerType = ContainerTrait::stored_container_type;
			using ExpectedContainerType = ContainerTrait::expected_container_type;

			const StoredContainerType container;

			between_wrapper_and_container() = delete; //禁用默认构造函数

			explicit constexpr between_wrapper_and_container(ExpectedContainerType container) noexcept
				: container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator, const StoredInstanceType& instance_j)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI > (instance_j) };
			}
			[[nodiscard]] constexpr bool satisfies(ValidatorType&& validator) const
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator()
						< typename List::template element<Index[0]> >
						(container[Index[1]]);
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <typename TypeI>(auto&& instance_j) -> bool { ... }");

				return false;
			}

		};

		template<valuewise::List ValueList, typename ContainerType>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_value_list<ValueList>::size, static_container_size<ContainerType> }
			>
		struct between_wrapper_and_container<ValueList, ContainerType>
		{
			using List = as_value_list<ValueList>;
			static constexpr auto ContainerSize = static_container_size<ContainerType>;
			static constexpr extent_type Extent{ List::size, ContainerSize };
			static constexpr auto ProbeIndex = probe_index<Arrangement, Extent>;
			static constexpr auto ProbeValueI = List::template element<ProbeIndex[0]>;

			using ContainerTrait = vector_like_container_trait<ContainerType>;

			using StoredInstanceType = ContainerTrait::stored_instance_type;
			using StoredContainerType = ContainerTrait::stored_container_type;
			using ExpectedContainerType = ContainerTrait::expected_container_type;

			const StoredContainerType container;

			between_wrapper_and_container() = delete; //禁用默认构造函数

			explicit constexpr between_wrapper_and_container(ExpectedContainerType container) noexcept
				: container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator, const StoredInstanceType& instance_j)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI > (instance_j) };
			}
			[[nodiscard]] constexpr bool satisfies(ValidatorType&& validator) const
			{
				return template_validation_loop<Quantifier, Arrangement, Extent>
					([&] <auto Index> { return
						validator.template operator()
						< List::template element<Index[0]> >
						(container[Index[1]]);
					});
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <auto ValueI>(auto&& instance_j) -> bool { ... }");

				return false;
			}

		};

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/vector_like.h>
#include <logicwise/external_detail/exosuit.h>
#include <logicwise/index/sampler.h>
#include <logicwise/detail/vector_like_container_trait.h>
#include "validation_loop.h"
#include <ranges> //用于 std::ranges，C++20标准
#include <functional> //用于 std::invoke
#include <utility> //用于 std::forward


namespace logicwise::detail
{
	struct pairwise_validator;

	template<typename Quantifier, typename Arrangement>
	class pairwise_validation;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

	struct pairwise_validator
	{
		template<typename Quantifier, typename Arrangement, typename TypeList,
			template<typename, typename> typename Validator, typename TraitCertificate>
		static constexpr bool validate_type_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_type_list_with_invocable<Quantifier, Arrangement, TypeList>
					([] <typename TypeI, typename TypeJ> { return Validator<TypeI, TypeJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_type_list_with_invocable<Quantifier, Arrangement, TypeList>
					([] <typename TypeI, typename TypeJ> { return Validator<TypeI, TypeJ>{}; });
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
			constexpr typename Arrangement::extent_type Extent{ TypeList::size };

			return template_validation_loop<Quantifier, Arrangement, Extent>
				([&] <auto Index> { return
					validator.template operator() <
						typename TypeList::template element<Index[0]>,
						typename TypeList::template element<Index[1]>
					> ();
				});
		}

		template<typename Quantifier, typename Arrangement, typename ValueList,
			template<auto, auto> typename Validator, typename TraitCertificate>
		static constexpr bool validate_value_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_value_list_with_invocable<Quantifier, Arrangement, ValueList>
					([] <auto ValueI, auto ValueJ> { return Validator<ValueI, ValueJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_value_list_with_invocable<Quantifier, Arrangement, ValueList>
					([] <auto ValueI, auto ValueJ> { return Validator<ValueI, ValueJ>{}; });
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
			constexpr typename Arrangement::extent_type Extent{ ValueList::size };

			return template_validation_loop<Quantifier, Arrangement, Extent>
				([&] <auto Index> { return
					validator.template operator() <
						ValueList::template element<Index[0]>,
						ValueList::template element<Index[1]>
					> ();
				});
		}

		template<typename Quantifier, typename Arrangement,
			typename ContainerType, typename ValidatorType>
		static constexpr bool validate_container(const ContainerType& container, ValidatorType&& validator)
		{
			typename Arrangement::extent_type extent{ std::ranges::size(container) };

			return instance_validation_loop<Quantifier, Arrangement>(extent,
				[&] (auto&& index) { return
					std::invoke(validator, container[index[0]], container[index[1]]);
				});
		}

	};

	//================================================================================

	template<typename Quantifier, typename Arrangement>
	class pairwise_validation
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

			template<template<typename, typename> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return validation_result; }

			template<template<auto, auto> typename>
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

			using ProbeTypeI = typename List::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename List::template element<ProbeIndex[1]>;

			template<template<typename, typename> typename Validator>
				requires requires { typename Validator<ProbeTypeI, ProbeTypeJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeTypeI, ProbeTypeJ>;

				return pairwise_validator::template validate_type_list_with_trait
					<Quantifier, Arrangement, List, Validator, TraitCertificate>();
			}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI, ProbeTypeJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return pairwise_validator::template validate_type_list_with_invocable
					<Quantifier, Arrangement, List>
					(std::forward<ValidatorType>(validator));
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

			static constexpr auto ProbeValueI = List::template element<ProbeIndex[0]>;
			static constexpr auto ProbeValueJ = List::template element<ProbeIndex[1]>;

			template<template<auto, auto> typename Validator>
				requires requires { typename Validator<ProbeValueI, ProbeValueJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeValueI, ProbeValueJ>;

				return pairwise_validator::template validate_value_list_with_trait
					<Quantifier, Arrangement, List, Validator, TraitCertificate>();
			}
		
			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI, ProbeValueJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return pairwise_validator::template validate_value_list_with_invocable
					<Quantifier, Arrangement, List>
					(std::forward<ValidatorType>(validator));
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


		template<typename ContainerType>
		struct in_container
		{
			using ContainerTrait = vector_like_container_trait<ContainerType>;

			using StoredInstanceType	= ContainerTrait::stored_instance_type;
			using StoredContainerType	= ContainerTrait::stored_container_type;
			using ExpectedContainerType	= ContainerTrait::expected_container_type;

			const StoredContainerType container;

			in_container() = delete; //禁用默认构造函数

			explicit constexpr in_container(ExpectedContainerType container) noexcept
				: container{ ContainerTrait::cast_container(static_cast<ExpectedContainerType>(container)) } {}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator,
			const StoredInstanceType& instance_i, const StoredInstanceType& instance_j)
			{
				bool{ std::invoke(std::forward<ValidatorType>(validator), instance_i, instance_j) };
			}
			[[nodiscard]] constexpr bool satisfies(ValidatorType&& validator) const
			{
				return pairwise_validator::template validate_container
					<Quantifier, Arrangement>
					(container, std::forward<ValidatorType>(validator));
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

	};

}

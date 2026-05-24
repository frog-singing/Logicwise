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
	struct bipartite_validator;

	template<typename Quantifier, typename Arrangement>
	class bipartite_validation;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::validation================================================================================

	struct bipartite_validator
	{
		template<typename Quantifier, typename Arrangement, typename TypeListA, typename TypeListB,
			template<typename, typename> typename Validator, typename TraitCertificate>
		static constexpr bool validate_type_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_type_list_with_invocable
					<Quantifier, Arrangement, TypeListA, TypeListB>
					([] <typename TypeI, typename TypeJ> { return Validator<TypeI, TypeJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_type_list_with_invocable
					<Quantifier, Arrangement, TypeListA, TypeListB>
					([] <typename TypeI, typename TypeJ> { return Validator<TypeI, TypeJ>{}; });
			}
			else
			{
				static_assert(dependent_false_v<TraitCertificate>,
					"[logicwise] Error: Unsupported trait validator! "
					"Certificate must provide '::value' or be convertible to bool via construction.");

				return false;
			}
		}

		template<typename Quantifier, typename Arrangement, typename TypeListA, typename TypeListB,
			typename ValidatorType>
		static constexpr bool validate_type_list_with_invocable(ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ TypeListA::size, TypeListB::size }),
				(
					validator_ref.template operator() <
						typename TypeListA::template element<Index[0]>,
						typename TypeListB::template element<Index[1]>
					> ()
				)
			);
		}

		template<typename Quantifier, typename Arrangement, typename ValueListA, typename ValueListB,
			template<auto, auto> typename Validator, typename TraitCertificate>
		static constexpr bool validate_value_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_value_list_with_invocable
					<Quantifier, Arrangement, ValueListA, ValueListB>
					([] <auto ValueI, auto ValueJ> { return Validator<ValueI, ValueJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_value_list_with_invocable
					<Quantifier, Arrangement, ValueListA, ValueListB>
					([] <auto ValueI, auto ValueJ> { return Validator<ValueI, ValueJ>{}; });
			}
			else
			{
				static_assert(dependent_false_v<TraitCertificate>,
					"[logicwise] Error: Unsupported trait validator! "
					"Certificate must provide '::value' or be convertible to bool via construction.");

				return false;
			}
		}

		template<typename Quantifier, typename Arrangement, typename ValueListA, typename ValueListB,
			typename ValidatorType>
		static constexpr bool validate_value_list_with_invocable(ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ ValueListA::size, ValueListB::size }),
				(
					validator_ref.template operator() <
						ValueListA::template element<Index[0]>,
						ValueListB::template element<Index[1]>
					> ()
				)
			);
		}

		template<typename Quantifier, typename Arrangement, typename TypeList, typename ValueList,
			template<typename, auto> typename Validator, typename TraitCertificate>
		static constexpr bool validate_type_list_and_value_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_type_list_and_value_list_with_invocable
					<Quantifier, Arrangement, TypeList, ValueList>
					([] <typename TypeI, auto ValueJ> { return Validator<TypeI, ValueJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_type_list_and_value_list_with_invocable
					<Quantifier, Arrangement, TypeList, ValueList>
					([] <typename TypeI, auto ValueJ> { return Validator<TypeI, ValueJ>{}; });
			}
			else
			{
				static_assert(dependent_false_v<TraitCertificate>,
					"[logicwise] Error: Unsupported trait validator! "
					"Certificate must provide '::value' or be convertible to bool via construction.");

				return false;
			}
		}

		template<typename Quantifier, typename Arrangement, typename TypeList, typename ValueList,
			typename ValidatorType>
		static constexpr bool validate_type_list_and_value_list_with_invocable(ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ TypeList::size, ValueList::size }),
				(
					validator_ref.template operator() <
						typename TypeList::template element<Index[0]>,
						ValueList::template element<Index[1]>
					> ()
				)
			);
		}

		template<typename Quantifier, typename Arrangement, typename ValueList, typename TypeList,
			template<auto, typename> typename Validator, typename TraitCertificate>
		static constexpr bool validate_value_list_and_type_list_with_trait()
		{
			if constexpr (requires { bool{ TraitCertificate::value }; })
			{
				return validate_value_list_and_type_list_with_invocable
					<Quantifier, Arrangement, ValueList, TypeList>
					([] <auto ValueI, typename TypeJ> { return Validator<ValueI, TypeJ>::value; });
			}
			else if constexpr (requires { bool{ TraitCertificate{} }; })
			{
				return validate_value_list_and_type_list_with_invocable
					<Quantifier, Arrangement, ValueList, TypeList>
					([] <auto ValueI, typename TypeJ> { return Validator<ValueI, TypeJ>{}; });
			}
			else
			{
				static_assert(dependent_false_v<TraitCertificate>,
					"[logicwise] Error: Unsupported trait validator! "
					"Certificate must provide '::value' or be convertible to bool via construction.");

				return false;
			}
		}

		template<typename Quantifier, typename Arrangement, typename ValueList, typename TypeList,
			typename ValidatorType>
		static constexpr bool validate_value_list_and_type_list_with_invocable(ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ ValueList::size, TypeList::size }),
				(
					validator_ref.template operator() <
						ValueList::template element<Index[0]>,
						typename TypeList::template element<Index[1]>
					> ()
				)
			);
		}

		//--------------------------------------------------------------------------------

		template<typename Quantifier, typename Arrangement,
			typename ContainerTypeA, typename ContainerTypeB, typename ValidatorType>
		static constexpr bool validate_container(
			const ContainerTypeA& containerA, const ContainerTypeB& containerB, ValidatorType&& validator)
		{
			using extent_type = typename Arrangement::extent_type;
			using index_traverser_type = typename Arrangement::fast_index_traverser;

			typename Quantifier::solver quantifier_solver{};

			extent_type extent{ std::ranges::size(containerA), std::ranges::size(containerB) };
			index_traverser_type index_traverser{ extent };

			//遍历入口处将 validator 固定为左值引用，避免不必要的复制
			auto& validator_ref = validator;

			while (!quantifier_solver.solved() && !index_traverser.done())
			{
				auto [i, j] = index_traverser.state();

				quantifier_solver.step
				(
					std::invoke(validator_ref, containerA[i], containerB[j])
				);

				index_traverser.step();
			}

			return quantifier_solver.result();
		}

		//--------------------------------------------------------------------------------

		template<typename Quantifier, typename Arrangement, typename TypeList,
			typename ContainerType, typename ValidatorType>
		static constexpr bool validate_type_list_and_container(
			const ContainerType& container, ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ TypeList::size, static_container_size<ContainerType> }),
				(
					validator_ref.template operator()
					< typename TypeList::template element<Index[0]> >
					(container[Index[1]])
				)
			);
		}

		template<typename Quantifier, typename Arrangement, typename ValueList,
			typename ContainerType, typename ValidatorType>
		static constexpr bool validate_value_list_and_container(
			const ContainerType& container, ValidatorType&& validator)
		{
			LOGICWISE_TEMPLATE_VALIDATION_LOOP
			(
				//宏函数中需要用 () 包裹初始化列表内部逗号
				({ ValueList::size, static_container_size<ContainerType> }),
				(
					validator_ref.template operator()
					< ValueList::template element<Index[0]> >
					(container[Index[1]])
				)
			);
		}

	};

	//================================================================================

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

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ ListA::size, ListB::size }
			> ;

			using ProbeTypeI = typename ListA::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename ListB::template element<ProbeIndex[1]>;

			template<template<typename, typename> typename Validator>
				requires requires { typename Validator<ProbeTypeI, ProbeTypeJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeTypeI, ProbeTypeJ>;

				return bipartite_validator::template validate_type_list_with_trait
					<Quantifier, Arrangement, ListA, ListB, Validator, TraitCertificate>();
			}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI, ProbeTypeJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return bipartite_validator::template validate_type_list_with_invocable
					<Quantifier, Arrangement, ListA, ListB>
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

		template<valuewise::List ValueListA, valuewise::List ValueListB>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_value_list<ValueListA>::size, as_value_list<ValueListB>::size }
			>
		struct between_wrapper<ValueListA, ValueListB>
		{
			using ListA = as_value_list<ValueListA>;
			using ListB = as_value_list<ValueListB>;

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ ListA::size, ListB::size }
			> ;

			static constexpr auto ProbeValueI = ListA::template element<ProbeIndex[0]>;
			static constexpr auto ProbeValueJ = ListB::template element<ProbeIndex[1]>;

			template<template<auto, auto> typename Validator>
				requires requires { typename Validator<ProbeValueI, ProbeValueJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeValueI, ProbeValueJ>;

				return bipartite_validator::template validate_value_list_with_trait
					<Quantifier, Arrangement, ListA, ListB, Validator, TraitCertificate>();
			}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI, ProbeValueJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return bipartite_validator::template validate_value_list_with_invocable
					<Quantifier, Arrangement, ListA, ListB>
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

		template<typewise::List TypeList, valuewise::List ValueList>
			requires TraversableExtent <
				Arrangement,
				extent_type{ as_type_list<TypeList>::size, as_value_list<ValueList>::size }
			>
		struct between_wrapper<TypeList, ValueList>
		{
			using ListA = as_type_list<TypeList>;
			using ListB = as_value_list<ValueList>;

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ ListA::size, ListB::size }
			> ;

			using ProbeTypeI = typename ListA::template element<ProbeIndex[0]>;
			static constexpr auto ProbeValueJ = ListB::template element<ProbeIndex[1]>;

			template<template<typename, auto> typename Validator>
				requires requires { typename Validator<ProbeTypeI, ProbeValueJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeTypeI, ProbeValueJ>;

				return bipartite_validator::template validate_type_list_and_value_list_with_trait
					<Quantifier, Arrangement, ListA, ListB, Validator, TraitCertificate>();
			}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeTypeI, ProbeValueJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return bipartite_validator::template validate_type_list_and_value_list_with_invocable
					<Quantifier, Arrangement, ListA, ListB>
					(std::forward<ValidatorType>(validator));
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

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ ListA::size, ListB::size }
			> ;

			static constexpr auto ProbeValueI = ListA::template element<ProbeIndex[0]>;
			using ProbeTypeJ = typename ListB::template element<ProbeIndex[1]>;

			template<template<auto, typename> typename Validator>
				requires requires { typename Validator<ProbeValueI, ProbeTypeJ>; }
			[[nodiscard]] static constexpr bool satisfies()
			{
				using TraitCertificate = Validator<ProbeValueI, ProbeTypeJ>;

				return bipartite_validator::template validate_value_list_and_type_list_with_trait
					<Quantifier, Arrangement, ListA, ListB, Validator, TraitCertificate>();
			}

			template<typename ValidatorType>
				requires requires(ValidatorType&& validator)
			{
				bool{ std::forward<ValidatorType>(validator)
					.template operator() < ProbeValueI, ProbeTypeJ > () };
			}
			[[nodiscard]] static constexpr bool satisfies(ValidatorType&& validator)
			{
				return bipartite_validator::template validate_value_list_and_type_list_with_invocable
					<Quantifier, Arrangement, ListA, ListB>
					(std::forward<ValidatorType>(validator));
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
				return bipartite_validator::template validate_container
					<Quantifier, Arrangement>
					(containerA, containerB, std::forward<ValidatorType>(validator));
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] (auto instance_i, auto instance_j) -> bool { ... }");

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

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ List::size, ContainerSize }
			> ;

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
				return bipartite_validator::template validate_type_list_and_container
					<Quantifier, Arrangement, List>
					(container, std::forward<ValidatorType>(validator));
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <typename TypeI>(auto instance_j) -> bool { ... }");

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

			static constexpr auto ProbeIndex = probe_index <
				Arrangement,
				extent_type{ List::size, ContainerSize }
			> ;

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
				return bipartite_validator::template validate_value_list_and_container
					<Quantifier, Arrangement, List>
					(container, std::forward<ValidatorType>(validator));
			}

			template<typename ValidatorType>
			static constexpr bool satisfies(ValidatorType&&)
			{
				static_assert(dependent_false_v<ValidatorType>,
					"[logicwise] Error: Incompatible validator signature!\n"
					"Expected: [] <auto ValueI>(auto instance_j) -> bool { ... }");

				return false;
			}

		};

	};

}

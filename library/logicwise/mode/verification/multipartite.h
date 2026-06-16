// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>


#include <utility> //用于 std::forward, std::move, std::declval


namespace logicwise::detail
{
	struct multipartite_verifier;

	template<typename Quantifier, typename Arrangement>
	class multipartite_verification;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::验证 mode::verification================================================================================
	
	struct multipartite_verifier
	{


	};

	//================================================================================

	template<typename Quantifier, typename Arrangement>
	class multipartite_verification : area_ahead<Quantifier, Arrangement>
	{
	public:
		template<typename... Emptiness>
			requires (sizeof...(Emptiness) == 0)
		[[nodiscard]] static constexpr auto among()
		{
			return among_emptiness{};
		}

		template<typename... WrapperInstance>
			requires (sizeof...(WrapperInstance) > 0)
		[[nodiscard]] static constexpr auto among()
		{
			return among_wrapper<WrapperInstance...>{};
		}

		template<typename... ContainerType>
			requires (sizeof...(ContainerType) > 0)
		[[nodiscard]] static constexpr auto among(ContainerType&&... container)
		{
			return among_container<ContainerType...>{ std::forward<ContainerType>(container)... };
		}

		template<typename WrapperInstanceBatch>
		[[nodiscard]] static constexpr auto in_batch()
		{
			return in_wrapper_batch<WrapperInstanceBatch>{};
		}

		template<typename ContainerBatchType>
		[[nodiscard]] static constexpr auto in_batch(ContainerBatchType&& container_batch)
		{
			return in_container_batch<ContainerBatchType>{ std::forward<ContainerBatchType>(container_batch) };
		}


	private:
		struct among_emptiness
		{
			static constexpr bool verification_result{ typename Quantifier::solver{}.result() };

			template<template<typename...> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return verification_result; }

			template<template<auto...> typename>
			[[nodiscard]] static constexpr bool satisfies() noexcept { return verification_result; }

			[[nodiscard]] static constexpr bool satisfies(auto&&) noexcept { return verification_result; }

			template<template<typename> typename>
			[[nodiscard]] static constexpr bool satisfies_rangewise() noexcept { return verification_result; }

			[[nodiscard]] static constexpr bool satisfies_rangewise(auto&&) noexcept { return verification_result; }
		};

		template<typename... WrapperInstance>
		struct among_wrapper;

		template<typename... ContainerType>
		struct among_container;

		template<typename WrapperInstanceBatch>
		struct in_wrapper_batch;

		template<typename ContainerBatchType>
		struct in_container_batch;

	};

}

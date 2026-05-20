// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array


namespace logicwise::detail
{
	template<std::size_t N>
	struct IndexTraitND;

	template<std::size_t N>
	struct IndexTraitPaddingND;

	template<std::size_t N>
	struct [[nodiscard]] ExtentND;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	//索引协议
	using IndexTrait1D = IndexTraitND<1>;
	using IndexTrait2D = IndexTraitND<2>;
	using IndexTrait3D = IndexTraitND<3>;

	template<>
	struct IndexTraitND<1>
	{
		using index_type = std::size_t;

		template<index_type... Index>
		struct index_sequence
		{
			template<typename Invocable>
			static constexpr decltype(auto) invoke(Invocable&& invocable)
			{
				return std::forward<Invocable>(invocable).template operator() < Index... > ();
			}
		};

	};

	template<std::size_t N>
	struct IndexTraitND
	{
		using index_type = std::array<std::size_t, N>;

		template<index_type... Index>
		struct index_sequence
		{
			template<typename Invocable>
			static constexpr decltype(auto) invoke(Invocable&& invocable)
			{
				return std::forward<Invocable>(invocable).template operator() < Index... > ();
			}
		};

	};
	
	//带填充索引协议
	using IndexTraitPadding2D = IndexTraitPaddingND<2>;

	template<std::size_t N>
	struct [[nodiscard]] IndexPaddingND
	{
		std::array<std::size_t, N> component{}; //分量
		std::array<bool, N> padding_state{}; //true 表示填充
	};

	template<std::size_t N>
	struct IndexTraitPaddingND
	{
		using index_type = IndexPaddingND<N>;

		template<index_type... Index>
		struct index_sequence
		{
			template<typename Invocable>
			static constexpr decltype(auto) invoke(Invocable&& invocable)
			{
				return std::forward<Invocable>(invocable).template operator() < Index... > ();
			}
		};

	};

	//规模结构体
	using Extent1D = ExtentND<1>;
	using Extent2D = ExtentND<2>;

	template<std::size_t N>
	struct [[nodiscard]] ExtentND
	{
		std::array<std::size_t, N> component{}; //分量


		[[nodiscard]] constexpr std::size_t& i() noexcept requires (N >= 1) { return component[0]; }
		[[nodiscard]] constexpr std::size_t i() const noexcept requires (N >= 1) { return component[0]; }

		[[nodiscard]] constexpr std::size_t& j() noexcept requires (N >= 2) { return component[1]; }
		[[nodiscard]] constexpr std::size_t j() const noexcept requires (N >= 2) { return component[1]; }

		[[nodiscard]] constexpr std::size_t& k() noexcept requires (N >= 3) { return component[2]; }
		[[nodiscard]] constexpr std::size_t k() const noexcept requires (N >= 3) { return component[2]; }


		[[nodiscard]] constexpr bool empty() const noexcept
		{
			for (auto value : component) { if (!value) return true; }
			return false;
		}

		[[nodiscard]] constexpr std::size_t volume() const noexcept
		{
			std::size_t volume = 1;
			for (auto value : component) { volume *= value; }
			return volume;
		}

		constexpr void zero() noexcept 
		{
			component.fill(0);
		}

	};

}

// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array


namespace logicwise::detail
{
	template<std::size_t Dimension>
	struct IndexTrait;

	template<std::size_t Dimension>
	struct IndexTraitPadding;

	template<std::size_t Dimension>
	struct [[nodiscard]] Extent;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	//索引协议
	using IndexTrait1D = IndexTrait<1>;
	using IndexTrait2D = IndexTrait<2>;
	using IndexTrait3D = IndexTrait<3>;

	template<>
	struct IndexTrait<1>
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

	template<std::size_t Dimension>
	struct IndexTrait
	{
		using index_type = std::array<std::size_t, Dimension>;

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
	using IndexTraitPadding2D = IndexTraitPadding<2>;

	template<std::size_t Dimension>
	struct [[nodiscard]] IndexPadding
	{
		std::array<std::size_t, Dimension> component{}; //分量
		std::array<bool, Dimension> padding_state{}; //true 表示填充
	};

	template<std::size_t Dimension>
	struct IndexTraitPadding
	{
		using index_type = IndexPadding<Dimension>;

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
	using Extent1D = Extent<1>;
	using Extent2D = Extent<2>;

	template<std::size_t Dimension>
	struct [[nodiscard]] Extent
	{
		std::array<std::size_t, Dimension> component{}; //分量


		[[nodiscard]] constexpr std::size_t& i() noexcept requires (Dimension >= 1) { return component[0]; }
		[[nodiscard]] constexpr std::size_t i() const noexcept requires (Dimension >= 1) { return component[0]; }

		[[nodiscard]] constexpr std::size_t& j() noexcept requires (Dimension >= 2) { return component[1]; }
		[[nodiscard]] constexpr std::size_t j() const noexcept requires (Dimension >= 2) { return component[1]; }

		[[nodiscard]] constexpr std::size_t& k() noexcept requires (Dimension >= 3) { return component[2]; }
		[[nodiscard]] constexpr std::size_t k() const noexcept requires (Dimension >= 3) { return component[2]; }


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

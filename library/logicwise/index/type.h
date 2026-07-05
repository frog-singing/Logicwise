// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <cstddef> //用于 std::size_t
#include <array> //用于 std::array
#include <iterator> //用于 std::reverse_iterator


namespace logicwise::detail
{
	template<typename IndexType>
	struct IndexProtocol;

	struct IndexTraitScalar;

	template<std::size_t Dimension>
	struct IndexTrait;

	template<std::size_t Dimension>
	struct IndexTraitPadding;

	template<std::size_t Dimension>
	struct [[nodiscard]] Extent;

	template<typename IndexType, std::size_t Size>
	struct ViewData;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	//索引协议
	template<typename IndexType>
	struct IndexProtocol
	{
		using index_type = IndexType;

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

	//标量索引特征
	struct IndexTraitScalar : IndexProtocol<
		std::size_t //index_type
	> {};

	//索引特征
	using IndexTrait0D = IndexTrait<0>;
	using IndexTrait1D = IndexTrait<1>;
	using IndexTrait2D = IndexTrait<2>;
	using IndexTrait3D = IndexTrait<3>;

	template<std::size_t Dimension>
	struct IndexTrait : IndexProtocol<
		std::array<std::size_t, Dimension> //index_type
	> {};

	//带填充索引特征
	using IndexTraitPadding2D = IndexTraitPadding<2>;

	template<std::size_t Dimension>
	struct [[nodiscard]] IndexPadding
	{
		std::array<std::size_t, Dimension> component{}; //分量
		std::array<bool, Dimension> padding_state{}; //true 表示填充

		[[nodiscard]] bool operator==(const IndexPadding&) const = default;
		[[nodiscard]] constexpr std::size_t operator[](std::size_t i) const noexcept { return component[i]; }
	};

	template<std::size_t Dimension>
	struct IndexTraitPadding : IndexProtocol<
		IndexPadding<Dimension> //index_type
	> {};

	//规模结构体
	using Extent0D = Extent<0>;
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


		/*
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
		*/

	};

	//视图数据
	template<typename IndexType, std::size_t MaxSize>
	struct [[nodiscard]] ViewData
	{
		std::array<IndexType, MaxSize> index_array{};
		std::size_t offset{};
		std::size_t size{};

		[[nodiscard]] constexpr IndexType operator[](std::size_t i) const noexcept { return index_array[offset + i]; }

		[[nodiscard]] constexpr auto begin() const noexcept { return index_array.begin() + offset; }
		[[nodiscard]] constexpr auto end() const noexcept { return index_array.begin() + offset + size; }

		[[nodiscard]] constexpr auto rbegin() const noexcept { return std::reverse_iterator{ index_array.begin() + offset + size }; }
		[[nodiscard]] constexpr auto rend() const noexcept { return std::reverse_iterator{ index_array.begin() + offset }; }
	};

	template<typename IndexType, std::size_t MaxSize>
	ViewData(std::array<IndexType, MaxSize>, std::size_t, std::size_t) -> ViewData<IndexType, MaxSize>;

}

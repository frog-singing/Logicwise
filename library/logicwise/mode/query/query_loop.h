// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/mode/traversal/traversal_loop.h>
#include <cstddef> //用于 std::size_t
#include <optional> //用于 std::optional
#include <functional> //用于 std::invoke


//逻辑维度::细节
namespace logicwise::detail
{
	//行为模式::查询 mode::query================================================================================

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicValidationType>
	static constexpr std::size_t template_count_loop(AtomicValidationType&& atomic_validation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		std::size_t count{};

		template_execute_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_validation.template operator() < Index > ()) { ++count; }
			});

		return count;
	}

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicValidationType>
	static constexpr auto template_find_first_loop(AtomicValidationType&& atomic_validation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::forward_index_traverser;

		std::optional<IndexType> found_index{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_validation.template operator() < Index > ())
				{
					found_index = Index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicValidationType>
	static constexpr auto template_find_last_loop(AtomicValidationType&& atomic_validation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::reverse_index_traverser;

		std::optional<IndexType> found_index{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_validation.template operator() < Index > ())
				{
					found_index = Index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename AtomicValidationType>
	static constexpr std::size_t instance_count_loop(typename Arrangement::extent_type extent,
		AtomicValidationType&& atomic_validation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		std::size_t count{};

		instance_execute_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_validation, index)) { ++count; }
			});

		return count;
	}

	template<typename Arrangement, typename AtomicValidationType>
	static constexpr auto instance_find_first_loop(typename Arrangement::extent_type extent,
		AtomicValidationType&& atomic_validation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::forward_index_traverser;

		std::optional<IndexType> found_index{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_validation, index))
				{
					found_index = index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename AtomicValidationType>
	static constexpr auto instance_find_last_loop(typename Arrangement::extent_type extent,
		AtomicValidationType&& atomic_validation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::reverse_index_traverser;

		std::optional<IndexType> found_index{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_validation, index))
				{
					found_index = index;
					return true;
				}
				return false;
			});

		return found_index;
	}

}

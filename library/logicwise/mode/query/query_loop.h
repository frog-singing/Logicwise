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

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicVerificationType>
	constexpr std::size_t template_count_loop(AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		std::size_t count{};

		template_execute_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_predicate_evaluation.template operator() < Index > ()) { ++count; }
			});

		return count;
	}

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicVerificationType>
	constexpr auto template_find_first_index_loop(AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::forward_index_traverser;

		std::optional<IndexType> found_index{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_predicate_evaluation.template operator() < Index > ())
				{
					found_index = Index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename Arrangement::extent_type Extent, typename AtomicVerificationType>
	constexpr auto template_find_last_index_loop(AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::reverse_index_traverser;

		std::optional<IndexType> found_index{};

		template_execute_until_loop<Arrangement, IndexTraverserType, Extent>
			([&] <auto Index> {
				if (atomic_predicate_evaluation.template operator() < Index > ())
				{
					found_index = Index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename AtomicVerificationType>
	constexpr std::size_t instance_count_loop(typename Arrangement::extent_type extent,
		AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexTraverserType = typename Arrangement::fast_index_traverser;

		std::size_t count{};

		instance_execute_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_predicate_evaluation, index)) { ++count; }
			});

		return count;
	}

	template<typename Arrangement, typename AtomicVerificationType>
	constexpr auto instance_find_first_index_loop(typename Arrangement::extent_type extent,
		AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::forward_index_traverser;

		std::optional<IndexType> found_index{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_predicate_evaluation, index))
				{
					found_index = index;
					return true;
				}
				return false;
			});

		return found_index;
	}

	template<typename Arrangement, typename AtomicVerificationType>
	constexpr auto instance_find_last_index_loop(typename Arrangement::extent_type extent,
		AtomicVerificationType&& atomic_predicate_evaluation)
	{
		using IndexType = typename Arrangement::index_type;
		using IndexTraverserType = typename Arrangement::reverse_index_traverser;

		std::optional<IndexType> found_index{};

		instance_execute_until_loop<Arrangement, IndexTraverserType>(extent,
			[&] (auto&& index) {
				if (std::invoke(atomic_predicate_evaluation, index))
				{
					found_index = index;
					return true;
				}
				return false;
			});

		return found_index;
	}

}

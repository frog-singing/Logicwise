// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/exosuit.h>
#include <cstddef> //用于 std::size_t
#include <utility> //用于 std::forward
#include <optional> //用于 std::optional


//逻辑维度::细节
namespace logicwise::detail
{
	//模式::查询 mode::query================================================================================

	template<typename QueryWorkshop>
	struct template_query_component
	{
		using material_agent	= typename QueryWorkshop::material_agent;
		using constraint		= typename QueryWorkshop::constraint;
		using default_content	= typename QueryWorkshop::default_content;
		using error_reporter	= typename QueryWorkshop::error_reporter;

		//--------------------------------------------------------------------------------

		using index_type = typename material_agent::index_type;
		static constexpr auto null_object = default_content::null_object;

		//================================================================================

		struct component
		{
			[[nodiscard]] static constexpr std::size_t count(auto&&) noexcept { return 0; }
			[[nodiscard]] static constexpr std::optional<index_type> find_first_index(auto&&) noexcept { return {}; }
			[[nodiscard]] static constexpr std::optional<index_type> find_last_index(auto&&) noexcept { return {}; }
			[[nodiscard]] static constexpr auto find_first(auto&&) noexcept { return null_object; }
			[[nodiscard]] static constexpr auto find_last(auto&&) noexcept { return null_object; }

			//不提供 get<ScalarIndex>()

			template<std::size_t ScalarIndex>
			static constexpr auto at()
			{
				static_assert(valuewise_dependent_false_v<ScalarIndex>, "[logicwise] Scalar index out of range.");

				return null_object;
			}

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{
			template<std::size_t ScalarIndex>
			static constexpr index_type index_at()
			{
				static_assert(valuewise_dependent_false_v<ScalarIndex>, "[logicwise] Scalar index out of range.");

				return {};
			}

			//不提供 lookup<Index>()

			template<auto Index>
				requires constraint::template index_with_valid_type<Index>
			static constexpr auto try_lookup()
			{
				static_assert(valuewise_dependent_false_v<Index>, "[logicwise] Index out of range.");

				return null_object;
			}

			//================================================================================

			template<auto Index>
			static constexpr auto try_lookup()
			{
				error_reporter::template incompatible_index<Index>();

				return null_object;
			}

		};

	};

	template<typename QueryWorkshop>
		requires QueryWorkshop::constraint::traversable_extent
	struct template_query_component<QueryWorkshop>
	{
		using material_agent	= typename QueryWorkshop::material_agent;
		using constraint		= typename QueryWorkshop::constraint;
		using query_executor	= typename QueryWorkshop::query_executor;
		using default_content	= typename QueryWorkshop::default_content;
		using error_reporter	= typename QueryWorkshop::error_reporter;

		template<typename Executor>
		static constexpr auto execute_query()
		{
			return QueryWorkshop::template execute_query<Executor>();
		}

		template<typename Executor, typename PredicateSolverType>
		static constexpr auto execute_query(PredicateSolverType&& predicate_solver)
		{
			return QueryWorkshop::template execute_query<Executor>
				(std::forward<PredicateSolverType>(predicate_solver));
		}

		//--------------------------------------------------------------------------------

		using index_type = typename material_agent::index_type;
		static constexpr auto null_object = default_content::null_object;

		template<typename Predicate>
		using predicate_solver = typename material_agent::template predicate_solver<Predicate>;

		//================================================================================

		struct component
		{
			template<typename PredicateType>
				requires constraint::template valid_predicate_type<PredicateType>
			[[nodiscard]] static constexpr std::size_t count(PredicateType&& predicate)
			{
				return execute_query< typename query_executor::count >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(predicate);
					});
			}

			template<typename PredicateType>
				requires constraint::template valid_predicate_type<PredicateType>
			[[nodiscard]] static constexpr std::optional<index_type> find_first_index(PredicateType&& predicate)
			{
				return execute_query< typename query_executor::find_first_index >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(predicate);
					});
			}

			template<typename PredicateType>
				requires constraint::template valid_predicate_type<PredicateType>
			[[nodiscard]] static constexpr std::optional<index_type> find_last_index(PredicateType&& predicate)
			{
				return execute_query< typename query_executor::find_last_index >
					([&] <auto Index> { return
						material_agent::template solve_predicate<Index>(predicate);
					});
			}

			template<typename Predicate>
				requires constraint::template valid_predicate<Predicate>
			[[nodiscard]] static constexpr auto find_first(Predicate&&)
			{
				return execute_query< typename query_executor::template find_first<
					default_content, predicate_solver<Predicate>
				> >();
			}

			template<typename Predicate>
				requires constraint::template valid_predicate<Predicate>
			[[nodiscard]] static constexpr auto find_last(Predicate&&)
			{
				return execute_query< typename query_executor::template find_last<
					default_content, predicate_solver<Predicate>
				> >();
			}

			template<std::size_t ScalarIndex>
			[[nodiscard]] static constexpr auto get()
			{
				return execute_query< typename query_executor::template get<ScalarIndex> >();
			}

			template<std::size_t ScalarIndex>
			[[nodiscard]] static constexpr auto at()
			{
				return execute_query< typename query_executor::template at<ScalarIndex> >();
			}

			//================================================================================

			template<typename PredicateType>
			static constexpr std::size_t count(PredicateType&&)
			{
				error_reporter::template incompatible_predicate<PredicateType>();

				return 0;
			}

			template<typename PredicateType>
			static constexpr std::optional<index_type> find_first_index(PredicateType&&)
			{
				error_reporter::template incompatible_predicate<PredicateType>();

				return {};
			}

			template<typename PredicateType>
			static constexpr std::optional<index_type> find_last_index(PredicateType&&)
			{
				error_reporter::template incompatible_predicate<PredicateType>();

				return {};
			}

			template<typename Predicate>
			static constexpr auto find_first(Predicate&&)
			{
				error_reporter::template incompatible_non_capturing_predicate<Predicate>();

				return null_object;
			}

			template<typename Predicate>
			static constexpr auto find_last(Predicate&&)
			{
				error_reporter::template incompatible_non_capturing_predicate<Predicate>();

				return null_object;
			}

		};

		//需要 constraint 提供对应的 component_with_index 内容
		struct component_with_index
		{
			template<std::size_t ScalarIndex>
			[[nodiscard]] static constexpr index_type index_at()
			{
				return execute_query< typename query_executor::template index_at<ScalarIndex> >();
			}

			template<auto Index>
				requires constraint::template index_with_valid_type<Index>
			[[nodiscard]] static constexpr auto lookup()
			{
				return execute_query< typename query_executor::template lookup<Index> >();
			}

			template<auto Index>
				requires constraint::template index_with_valid_type<Index>
			[[nodiscard]] static constexpr auto try_lookup()
			{
				return execute_query< typename query_executor::template try_lookup<Index> >();
			}

			//================================================================================

			template<auto Index>
			static constexpr auto lookup()
			{
				error_reporter::template incompatible_index<Index>();

				return null_object;
			}

			template<auto Index>
			static constexpr auto try_lookup()
			{
				error_reporter::template incompatible_index<Index>();

				return null_object;
			}

		};

	};

}

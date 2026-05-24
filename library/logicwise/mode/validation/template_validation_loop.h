// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <version> //用于 __cpp_expansion_statements (C++26)，C++20标准
#include <logicwise/external_detail/cxx_standard.h>


//LOGICWISE_TEMPLATE_VALIDATION_LOOP 需要外部环境提供 Quantifier, Arrangement, validator
//atomic_validation_in_parentheses 处，内部环境提供 validator_ref, Index
#if defined(__cpp_expansion_statements) && LOGICWISE_CXX_STANDARD >= LOGICWISE_CXX_26
	//C++26
	#define LOGICWISE_TEMPLATE_VALIDATION_LOOP(extent_initializer_list, atomic_validation_in_parentheses)	\
																											\
		using extent_type = typename Arrangement::extent_type;												\
		using index_traverser_type = typename Arrangement::fast_index_traverser;							\
																											\
		constexpr extent_type extent extent_initializer_list;												\
																											\
		constexpr auto index_array =																		\
			index_sequencer<Arrangement, index_traverser_type, extent>::generate_index_array();				\
																											\
		typename Quantifier::solver quantifier_solver{};													\
																											\
		/*遍历入口处将validator固定为左值引用，避免不必要的复制*/												\
		auto& validator_ref = validator;																	\
																											\
		template for (constexpr auto Index : index_array)													\
		{																									\
			if (quantifier_solver.solved()) { break; }														\
																											\
			quantifier_solver.step atomic_validation_in_parentheses;										\
		}																									\
																											\
		return quantifier_solver.result()
#else
	//C++20
	#define LOGICWISE_TEMPLATE_VALIDATION_LOOP(extent_initializer_list, atomic_validation_in_parentheses)	\
																											\
		using extent_type = typename Arrangement::extent_type;												\
		using index_traverser_type = typename Arrangement::fast_index_traverser;							\
																											\
		constexpr extent_type extent extent_initializer_list;												\
																											\
		using index_sequence =																				\
			typename index_sequencer<Arrangement, index_traverser_type, extent>::index_sequence;			\
																											\
		typename Quantifier::solver quantifier_solver{};													\
																											\
		/*遍历入口处将validator固定为左值引用，避免不必要的复制*/												\
		auto& validator_ref = validator;																	\
																											\
		index_sequence::invoke([&] <auto... Index> {														\
			(..., (																							\
				quantifier_solver.solved() ?																\
				void() : quantifier_solver.step atomic_validation_in_parentheses							\
			));																								\
		});																									\
																											\
		return quantifier_solver.result()
#endif

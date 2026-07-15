// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
    //细节 detail================================================================================

    //用于显式标记不应被洗回原始类型的类型，用作默认占位符
    struct do_not_launder_my_type_t {};
    inline constexpr do_not_launder_my_type_t do_not_launder_my_type{};

	template<auto V>
	concept DoNotLaunderMyType = std::same_as<decltype(V), do_not_launder_my_type_t>;

	//用于显式标记应尽可能保持自身类型的类型，用作默认占位符
	struct try_to_keep_my_type_t {};
	inline constexpr try_to_keep_my_type_t try_to_keep_my_type{};

	template<auto V>
	concept TryToKeepMyType = std::same_as<decltype(V), try_to_keep_my_type_t>;

	//用于显式标记应尽可能信任已有关系的场景，用作默认占位符
	struct try_to_trust_existing_relation_t {};
	inline constexpr try_to_trust_existing_relation_t try_to_trust_existing_relation{};

	template<auto V>
	concept TryToTrustExistingRelation = std::same_as<decltype(V), try_to_trust_existing_relation_t>;

}

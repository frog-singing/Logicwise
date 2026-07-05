// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <concepts> //用于 std::same_as，C++20标准


//逻辑维度::细节
namespace logicwise::detail
{
	//索引 index================================================================================

	template<typename Arrangement, auto Extent>
	concept ValidExtent =
		std::same_as<typename Arrangement::extent_type, decltype(Extent)> &&
		(
			!(requires { Arrangement::template ValidExtent<Extent>; }) ||
			Arrangement::template ValidExtent<Extent>
		);

}

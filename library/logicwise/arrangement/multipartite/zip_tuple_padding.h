// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/arrangement/type.h>
#include <logicwise/index/type.h>
#include <logicwise/semantics/guard/safe_integer_cast.h>
#include <cstddef> //用于 std::size_t
#include <cassert> //用于 assert


namespace logicwise::detail
{
	struct zip_tuple_padding;

	template<std::size_t Arity>
	struct zip_tuple_padding_impl;
}


//逻辑维度::细节
namespace logicwise::detail
{
	//排布::多部 arrangement::multipartite================================================================================

	//填充对齐多元组
	struct zip_tuple_padding : multipartite_arrangement_tag
	{
		template<std::size_t Arity>
		using arrangement = zip_tuple_padding_impl<Arity>;
	};

	template<std::size_t Arity>
	struct zip_tuple_padding_impl : arrangement_with_padding_tag
	{
		using extent_type = Extent<Arity>;
		using index_trait = IndexTraitPadding<Arity>;
		using index_type = typename index_trait::index_type;
		using index_integer_type = int;


		//使用 const extent_type&
		//使用 const index_type&
	};

}

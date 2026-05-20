// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/rangewise.h>
#include <logicwise/arrangement.h>
#include <logicwise/wrapper/range.h>
#include <logicwise/wrapper/set.h>
#include "homogeneous_relation.h"


//关系::互斥 relation::exclusion================================================================================

//逻辑维度::细节
namespace logicwise::detail
{
	//不相关范围
	template<typename A, typename B, auto RelatesTo>
	concept IrrelevantRangeTo =
		rangewise<none_of, cartesian_pair>
		::template between<A, B>()
		.satisfies(RelatesTo);

}

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
	//不相交范围，A 和 B 均为类型范围
	template<typename A, typename B, auto TolerantOf = default_tolerance_relation>
	concept DisjointRangeFrom = typewise::Range<A> && typewise::Range<B> && IrrelevantRangeTo<A, B, TolerantOf>;

	//不相交集合
	template<typename A, typename B, auto SameAs = default_equivalence_relation>
	concept DisjointSetFrom = Set<A, SameAs> && Set<B, SameAs> && IrrelevantRangeTo<A, B, SameAs>;

}

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
	//不相交范围，A 和 B 均为值范围
	template<typename A, typename B, auto TolerantOf = default_tolerance_relation>
	concept DisjointRangeFrom = valuewise::Range<A> && valuewise::Range<B> && IrrelevantRangeTo<A, B, TolerantOf>;

	//不相交集合
	template<typename A, typename B, auto EqualTo = default_equivalence_relation>
	concept DisjointSetFrom = Set<A, EqualTo> && Set<B, EqualTo> && IrrelevantRangeTo<A, B, EqualTo>;

}


//逻辑维度::类型维度
namespace logicwise::typewise
{
	using detail::typewise::DisjointRangeFrom;

	using detail::typewise::DisjointSetFrom;

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
	using detail::valuewise::DisjointRangeFrom;

	using detail::valuewise::DisjointSetFrom;

}

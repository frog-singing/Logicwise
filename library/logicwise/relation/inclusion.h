// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include "inclusion/type_inclusion.h"
#include "inclusion/value_inclusion.h"


//关系::包含 relation::inclusion================================================================================

//逻辑维度::类型维度
namespace logicwise::typewise
{
    using detail::typewise::BelongsTo;
    using detail::typewise::UniqueIn;
    using detail::typewise::UnrepeatedIn;

    using detail::typewise::SubRangeOf;                 //偏序关系 Partial Order
    using detail::typewise::UniquelyContainedIn;
    using detail::typewise::UnrepeatedlyContainedIn;

    using detail::typewise::SubSetOfRange;              //偏序关系 Partial Order
    using detail::typewise::RepresentativeSetOf;        //偏序关系 Partial Order
    using detail::typewise::SubSetOf;                   //偏序关系 Partial Order
    using detail::typewise::ProperSubSetOf;             //严格偏序关系 Strict Partial Order

    using detail::typewise::SameRangeAs;                //等价关系 Equivalence Relation
    using detail::typewise::SameSetAs;                  //等价关系 Equivalence Relation
    using detail::typewise::SameSetoidAs;               //等价关系 Equivalence Relation

}

//逻辑维度::值维度
namespace logicwise::valuewise
{
    using detail::valuewise::BelongsTo;
    using detail::valuewise::UniqueIn;
    using detail::valuewise::UnrepeatedIn;

    using detail::valuewise::SubRangeOf;                //偏序关系 Partial Order
    using detail::valuewise::UniquelyContainedIn;
    using detail::valuewise::UnrepeatedlyContainedIn;

    using detail::valuewise::SubSetOfRange;             //偏序关系 Partial Order
    using detail::valuewise::RepresentativeSetOf;       //偏序关系 Partial Order
    using detail::valuewise::SubSetOf;                  //偏序关系 Partial Order
    using detail::valuewise::ProperSubSetOf;            //严格偏序关系 Strict Partial Order

    using detail::valuewise::SameRangeAs;               //等价关系 Equivalence Relation
    using detail::valuewise::SameSetAs;                 //等价关系 Equivalence Relation
    using detail::valuewise::SameSetoidAs;              //等价关系 Equivalence Relation

}

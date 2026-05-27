// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#pragma once
#include <logicwise/external_detail/list.h>
#include <logicwise/external_detail/quantifier.h>
#include <logicwise/rangewise.h>
#include <logicwise/arrangement.h>
#include <logicwise/relation/homogeneous_relation.h>
#include <logicwise/wrapper/set.h>


//关系::包含 relation::inclusion================================================================================

//逻辑维度::细节::值维度
namespace logicwise::detail::valuewise
{
    //属于
    template<auto Value, typename Range, auto EqualTo = default_equivalence_relation>
    concept BelongsTo =
        rangewise<at_least<1>, element>
        ::template in<Range>()
        .satisfies([] <auto Element> { return EqualTo.template operator() < Value, Element > (); });

    //唯一存在于（同时满足属于）
    template<auto Value, typename Range, auto EqualTo = default_equivalence_relation>
    concept UniqueIn =
        rangewise<exactly<1>, element>
        ::template in<Range>()
        .satisfies([] <auto Element> { return EqualTo.template operator() < Value, Element > (); });

    //不重复存在于
    template<auto Value, typename Range, auto EqualTo = default_equivalence_relation>
    concept UnrepeatedIn =
        rangewise<at_most<1>, element>
        ::template in<Range>()
        .satisfies([] <auto Element> { return EqualTo.template operator() < Value, Element > (); });

    //--------------------------------------------------------------------------------

    //子范围
    template<typename SubRange, typename SuperRange, auto EqualTo = default_equivalence_relation>
    concept SubRangeOf =
        rangewise<all_of, element>
        ::template in<SubRange>()
        .satisfies([] <auto Element> { return BelongsTo<Element, SuperRange, EqualTo>; });

    //被唯一包含于（同时满足子范围）
    template<typename TargetRange, typename SourceRange, auto EqualTo = default_equivalence_relation>
    concept UniquelyContainedIn =
        rangewise<all_of, element>
        ::template in<TargetRange>()
        .satisfies([] <auto Element> { return UniqueIn<Element, SourceRange, EqualTo>; });

    //被不重复包含于
    template<typename TargetRange, typename SourceRange, auto EqualTo = default_equivalence_relation>
    concept UnrepeatedlyContainedIn =
        rangewise<all_of, element>
        ::template in<TargetRange>()
        .satisfies([] <auto Element> { return UnrepeatedIn<Element, SourceRange, EqualTo>; });

    //--------------------------------------------------------------------------------

    //范围子集合
    template<typename SubSet, typename SuperRange, auto EqualTo = default_equivalence_relation>
    concept SubSetOfRange =
        (as_value_list<SubSet>::size <= as_value_list<SuperRange>::size) &&
        SubRangeOf<SubSet, SuperRange, EqualTo> &&
        UnrepeatedlyContainedIn<SuperRange, SubSet, EqualTo>;

    //代表元集合（同时满足相同范围）
    template<typename RepresentativeSet, typename SuperRange, auto EqualTo = default_equivalence_relation>
    concept RepresentativeSetOf =
        (as_value_list<RepresentativeSet>::size <= as_value_list<SuperRange>::size) &&
        SubRangeOf<RepresentativeSet, SuperRange, EqualTo> &&
        UniquelyContainedIn<SuperRange, RepresentativeSet, EqualTo>;

    //子集
    template<typename SubSet, typename SuperSet, auto EqualTo = default_equivalence_relation>
    concept SubSetOf =
        (as_value_list<SubSet>::size <= as_value_list<SuperSet>::size) &&
        Set<SubSet, EqualTo> && Set<SuperSet, EqualTo> &&
        SubRangeOf<SubSet, SuperSet, EqualTo>;

    //真子集
    template<typename SubSet, typename SuperSet, auto EqualTo = default_equivalence_relation>
    concept ProperSubSetOf =
        (as_value_list<SubSet>::size < as_value_list<SuperSet>::size) &&
        Set<SubSet, EqualTo> && Set<SuperSet, EqualTo> &&
        SubRangeOf<SubSet, SuperSet, EqualTo>;

    //--------------------------------------------------------------------------------

    //相同范围
    template<typename A, typename B, auto EqualTo = default_equivalence_relation>
    concept SameRangeAs = SubRangeOf<A, B, EqualTo> && SubRangeOf<B, A, EqualTo>;

    //相同集合，外延等价 extensional equivalence
    template<typename A, typename B, auto EqualTo = default_equivalence_relation>
    concept SameSetAs =
        (as_value_list<A>::size == as_value_list<B>::size) &&
        Set<A, EqualTo> && Set<B, EqualTo> &&
        SubRangeOf<A, B, EqualTo>;

    //相同等价关系集合，内涵等价 intensional equivalence
    template<typename A, typename B>
    concept SameSetoidAs = ProvenSet<A> && ProvenSet<B> &&
        (as_value_list<A>::size == as_value_list<B>::size) &&
        valuewise::equal_to<A::equivalence_relation, B::equivalence_relation> &&
        SubRangeOf<A, B, A::equivalence_relation>;

}

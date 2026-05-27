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

//逻辑维度::细节::类型维度
namespace logicwise::detail::typewise
{
    //属于
    template<typename Type, typename Range, auto SameAs = default_equivalence_relation>
    concept BelongsTo =
        rangewise<at_least<1>, element>
        ::template in<Range>()
        .satisfies([] <typename Element> { return SameAs.template operator() < Type, Element > (); });

    //唯一存在于（同时满足属于）
    template<typename Type, typename Range, auto SameAs = default_equivalence_relation>
    concept UniqueIn =
        rangewise<exactly<1>, element>
        ::template in<Range>()
        .satisfies([] <typename Element> { return SameAs.template operator() < Type, Element > (); });

    //不重复存在于
    template<typename Type, typename Range, auto SameAs = default_equivalence_relation>
    concept UnrepeatedIn =
        rangewise<at_most<1>, element>
        ::template in<Range>()
        .satisfies([] <typename Element> { return SameAs.template operator() < Type, Element > (); });

    //--------------------------------------------------------------------------------

    //子范围
    template<typename SubRange, typename SuperRange, auto SameAs = default_equivalence_relation>
    concept SubRangeOf =
        rangewise<all_of, element>
        ::template in<SubRange>()
        .satisfies([] <typename Element> { return BelongsTo<Element, SuperRange, SameAs>; });

    //被唯一包含于（同时满足子范围）
    template<typename TargetRange, typename SourceRange, auto SameAs = default_equivalence_relation>
    concept UniquelyContainedIn =
        rangewise<all_of, element>
        ::template in<TargetRange>()
        .satisfies([] <typename Element> { return UniqueIn<Element, SourceRange, SameAs>; });

    //被不重复包含于
    template<typename TargetRange, typename SourceRange, auto SameAs = default_equivalence_relation>
    concept UnrepeatedlyContainedIn =
        rangewise<all_of, element>
        ::template in<TargetRange>()
        .satisfies([] <typename Element> { return UnrepeatedIn<Element, SourceRange, SameAs>; });

    //--------------------------------------------------------------------------------

    //范围子集合
    template<typename SubSet, typename SuperRange, auto SameAs = default_equivalence_relation>
    concept SubSetOfRange =
        (as_type_list<SubSet>::size <= as_type_list<SuperRange>::size) &&
        SubRangeOf<SubSet, SuperRange, SameAs> &&
        UnrepeatedlyContainedIn<SuperRange, SubSet, SameAs>;

    //代表元集合（同时满足相同范围）
    template<typename RepresentativeSet, typename SuperRange, auto SameAs = default_equivalence_relation>
    concept RepresentativeSetOf =
        (as_type_list<RepresentativeSet>::size <= as_type_list<SuperRange>::size) &&
        SubRangeOf<RepresentativeSet, SuperRange, SameAs> &&
        UniquelyContainedIn<SuperRange, RepresentativeSet, SameAs>;

    //子集
    template<typename SubSet, typename SuperSet, auto SameAs = default_equivalence_relation>
    concept SubSetOf =
        (as_type_list<SubSet>::size <= as_type_list<SuperSet>::size) &&
        Set<SubSet, SameAs> && Set<SuperSet, SameAs> &&
        SubRangeOf<SubSet, SuperSet, SameAs>;

    //真子集
    template<typename SubSet, typename SuperSet, auto SameAs = default_equivalence_relation>
    concept ProperSubSetOf =
        (as_type_list<SubSet>::size < as_type_list<SuperSet>::size) &&
        Set<SubSet, SameAs> && Set<SuperSet, SameAs> &&
        SubRangeOf<SubSet, SuperSet, SameAs>;

    //--------------------------------------------------------------------------------

    //相同范围
    template<typename A, typename B, auto SameAs = default_equivalence_relation>
    concept SameRangeAs = SubRangeOf<A, B, SameAs> && SubRangeOf<B, A, SameAs>;

    //相同集合，外延等价 extensional equivalence
    template<typename A, typename B, auto SameAs = default_equivalence_relation>
    concept SameSetAs =
        (as_type_list<A>::size == as_type_list<B>::size) &&
        Set<A, SameAs> && Set<B, SameAs> &&
        SubRangeOf<A, B, SameAs>;

    //相同等价关系集合，内涵等价 intensional equivalence
    template<typename A, typename B>
    concept SameSetoidAs = ProvenSet<A> && ProvenSet<B> &&
        (as_type_list<A>::size == as_type_list<B>::size) &&
        valuewise::equal_to<A::equivalence_relation, B::equivalence_relation> &&
        SubRangeOf<A, B, A::equivalence_relation>;

}

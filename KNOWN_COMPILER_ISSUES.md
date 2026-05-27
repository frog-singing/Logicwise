# Logicwise Known Compiler Issues

This document records known compiler issues encountered by the Logicwise test suite on mainstream C++20 compilers.
The affected cases are concentrated around template metaprogramming,
constrained partial specializations, class-type NTTPs, pointer NTTPs, and `nullptr` NTTPs.

These issues do not necessarily mean the Logicwise tests are wrong.
Many of the tests intentionally exercise C++20 language boundaries,
so they are more likely to expose frontend differences between compilers.

This document was written with the help of AI and does not imply that the author fully understands compiler mechanisms.

## Requirements

Logicwise requires C++20.

Compilers used for this testing:

| Compiler          | Version       | Results               |
| ---               | ---           | ---                   |
| MSVC              | 19.51.36246   | [Issue 1](#issue_1)   |
| Clang (clang-cl)  | 22.1.3        | [Issue 2](#issue_2)   |
| Clang (clang++)   | 22.1.3        | [Issue 2](#issue_2)   |
| GCC               | 16.1.0        | All tests passed      |

## Issue 1: MSVC pointer NTTP identity difference in `cv_ref_ptr_matrix.cpp` <a id="issue_1"></a>

Affected file:

```text
test/degeneracy/cv_ref_ptr_matrix.cpp
```

Symptom:

The file already marks the known MSVC difference compared to Clang/GCC:

```cpp
valuewise::same_as<V, ptr>; // same_as for Clang/GCC, !same_as for MSVC
```

The affected values are similar to:

```cpp
constexpr int* ptr{ nullptr };

value_list<
    (int*)(ptr),
    (int* const)(ptr),
    (int* volatile)(ptr),
    (int* const volatile)(ptr),
    (int*&)(ptr),
    (int*&&)(ptr),
    (int* const&)(ptr),
    (int* const&&)(ptr)
>
```

Logicwise/Manipond checks value identity with a pattern similar to:

```cpp
template<auto, auto>
constexpr bool is_same_value_v = false;

template<auto V>
constexpr bool is_same_value_v<V, V> = true;
```

Clang and GCC treat these expressions as the same `int*` null pointer value
after NTTP normalization and therefore satisfy `valuewise::same_as<V, ptr>`.
MSVC may evaluate the same check as `false`.

Assessment:

This is highly likely to be an MSVC difference or bug in pointer NTTP template-argument equivalence.
It may be preserving cv/ref conversion details or expression provenance in a way that affects template value identity;
or, it might just treat the pointer values of different indices in the template as values of different identities.

Workarounds:

- Follow the comments in `cv_ref_ptr_matrix.cpp` and adjust the affected assertions for MSVC.
- Or use a looser value-equivalence check for MSVC, for example:

```cpp
V == nullptr
```

or `valuewise::equal_to`, depending on the intent of the test.

## Issue 2: Clang order-sensitive constrained partial specialization failure in `quantifier.cpp` <a id="issue_2"></a>

Affected file:

```text
test/mode/validation/quantifier.cpp
```

Observed environment:

```text
clang 22.1.6, x86_64-w64-windows-gnu
```

Symptom:

Both of the following assertions compile when compiled independently.

```cpp
static_assert(
    rangewise<all_of, element>
    ::in<type_list<int, int, int, int>>()
    .satisfies([] <typename T> { return std::same_as<T, int>; }),
    "quantifier semantics: all_of"
);
```

```cpp
static_assert(
    rangewise<all_of, element>
    ::in<type_list<>>()
    .satisfies([] <typename T> { return std::same_as<T, int>; }),
    "quantifier vacuity: all_of"
);
```

The empty-list assertion also compiles if it appears before the non-empty assertion.
However, when the non-empty assertion is instantiated first,
Clang may incorrectly instantiate the specialization
intended only for traversable ranges and fail with a diagnostic similar to:

```text
constraints not satisfied for alias template 'element'
Index = 18446744073709551615
```

Analysis:

Logicwise uses a traversability constraint similar to:

```cpp
template<typename Arrangement, auto Extent>
concept TraversableExtent =
    std::same_as<typename Arrangement::extent_type, decltype(Extent)> &&
    !(typename Arrangement::light_index_traverser{ Extent }.done());
```

For an empty `type_list<>`, this constraint should be `false`,
so the specialization requiring `ProbeIndex` should not be selected. A direct check confirms this:

```cpp
static_assert(!TraversableExtent<arrangement, extent_type{ as_type_list<type_list<>>::size }>);
```

That assertion passes. Therefore the constraint itself evaluates to `false`,
but Clang still selects the constrained partial specialization under a particular instantiation order.

Assessment:

This is highly likely to be a Clang frontend issue involving the interaction of:

```text
constrained member class template partial specialization
class-type non-type template parameter
dependent alias adapter
prior instantiation with another list size
```

Workarounds:

- Compile this test with GCC or MSVC.
- Or add an explicit non-empty guard to the affected specialization:

```cpp
requires TraversableExtent<Arrangement, extent_type{ as_type_list<TypeList>::size }>
      && (!as_type_list<TypeList>::empty)
```

The guard is logically redundant, but it avoids Clang's bad instantiation path.

## Recommended Troubleshooting Steps

If a user hits a compile failure:

1. Confirm that C++20 is enabled.
2. Check whether the failing file is one of the three test files listed above.
3. If `cv_ref_ptr_matrix.cpp` fails with MSVC, follow the inline comments in that file.
4. If `quantifier.cpp` fails with Clang, suspect the constrained partial-specialization issue.
5. If the goal is to consume Logicwise rather than validate compiler behavior, disable tests:

```bash
cmake -DLOGICWISE_BUILD_TESTING=OFF ...
```

## Current Recommendation

Logicwise itself is usable as a header-only library on mainstream compilers.
The known failures are concentrated in tests that intentionally stress C++20 metaprogramming edge cases.

If you only need to integrate the library, disable the tests.
If you are developing Logicwise or investigating compiler behavior,
keep the tests and apply the compiler-specific notes above.

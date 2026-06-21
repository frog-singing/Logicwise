<div align="center">

# Logicwise

*Factory for C++ Concepts*

English | [简体中文](README.zh-Hans.md)

</div>

<!-------------------------------------------------------------------------------->

## 🔍 Syntax Overview

```cpp
rangewise<all_of, cartesian_pair>
	::between<RangeA, RangeB>()
	.satisfies(predicate);
```

Which translates as:

> Rangewise, all of the cartesian pairs between range A and range B satisfy the given predicate P.
> 
> ∀x ∈ A, ∀y ∈ B, P(x, y)  

Or:

> ∀(x, y) ∈ A×B, P(x, y)

<div align="center">

Orthogonally compose your logic:

<table>
	<tr>
	    <td style="padding: 14px 28px;" align="center">
			<strong style="font-size: 1.5em">
				Quantifier × Arrangement × Range × Predicate
			</strong>
		</td>
	</tr>
</table>

*Verify as you declare. Let the code speak for itself.*

</div>

<!-------------------------------------------------------------------------------->

## 🤔 Design Philosophy

- **Speak Human**:
Directly model your logic through intuitive syntax, requiring only a fraction of mathematical thinking.

- **Orthogonal Architecture**:
Quantifiers, arrangements, ranges, and predicates are mutually independent, freely combinable, and highly extensible.

- **Uniform Syntax**:
Learn one syntax; rule them all.
Whether you are verifying types, values, or instances, the syntax remains consistent and smooth.

- **Interspecific Hybridization**:
In bipartite verification, type containers, value containers, and instance containers can be seamlessly mixed and matched,
breaking down the reproductive isolation of C++.

- **Zero-Cost Abstraction**:
Pure compile-time logic verification collapses directly into a boolean constant, introducing zero runtime overhead.
Meanwhile, it theoretically minimizes unnecessary compilation overhead.

<!-------------------------------------------------------------------------------->

## 📖 Quick Start

```cpp
#include <logicwise.h>

#include <array>
#include <concepts>
#include <iostream>


int main()
{
	using namespace logicwise; // rangewise
	using namespace logicwise::quantifier; // all_of, any_of, none_of, exactly<N> ...
	using namespace logicwise::mode; // for_each, in_reverse_for_each
	using namespace logicwise::arrangement; // element, combination_pair, linear_adjacent_pair, cartesian_pair, zip_pair_truncation ...
	using namespace logicwise::wrapper; // type_list, value_list

	// verification mode --------------------------------------------------------------------------------

	static_assert(
		rangewise<none_of, element>
		::in<type_list<double, void, int&, type_list<int>, double>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"Rangewise, none of the elements in the type list satisfy 'being the same type as int'."
	);

	static_assert(
		rangewise<any_of, combination_pair>
		::in<value_list<1, 2, 3, 4>>()
		.satisfies([] <auto V1, auto V2> { return (V1 + V2) == 5; }),
		"Rangewise, there exists a combination pair in the value list that satisfies 'the sum of the two values equals 5'."
	);

	static_assert(
		rangewise<exactly<9>, cartesian_pair>
		::between<value_list<true, 2, 3.0>>(std::array{ 1, 2, 3, 4 })
		.satisfies([] <auto V1>(auto&& v2) { return V1 != v2; }),
		"Rangewise, exactly 9 cartesian pairs between the value list and the array satisfy 'the two values are not equal'."
	);
	
	// traversal mode --------------------------------------------------------------------------------

	constexpr int count_1 = []
		{
			int i{ 0 };

			rangewise<for_each, element>
				::in(std::array{ 1, 2, 3, 4 })
				.execute([&] (auto&& v) { if (v % 2 == 0) { ++i; } });

			return i;
		}();

	static_assert(count_1 == 2, "Rangewise, there are 2 even numbers in the array.");

	int count_2{ 0 };

	rangewise<for_each, linear_adjacent_pair>
		::in<type_list<bool, int, double, double, char>>()
		.execute_until([&] <typename T1, typename T2> {
			if constexpr (std::same_as<T1, T2>) { return true; }
			++count_2;
			std::cout << "<" << typeid(T1).name() << ", " << typeid(T2).name() << ">" << std::endl;
			return false;
		});

	std::cout << "count: " << count_2 << std::endl << std::endl;

	/*
	output:
	<bool, int>
	<int, double>
	count: 2
	*/

	int count_3{ 0 };

	rangewise<in_reverse_for_each, zip_pair_truncation>
		::between<type_list<bool, int, double, char, void>, value_list<true, 2u, 3.0, 4>>()
		.execute([&] <typename T, auto V> {
			if constexpr (!std::same_as<T, decltype(V)>) { return; }
			++count_3;
			std::cout << "<" << typeid(T).name() << ", " << V << ">" << std::endl;
		});

	std::cout << "count: " << count_3 << std::endl << std::endl;
	
	/*
	output:
	<double, 3>
	<bool, 1>
	count: 2
	*/

	std::cout << "Hello Logic!" << std::endl;
}
```

<!-------------------------------------------------------------------------------->

## 🔌 Integration

### Prerequisites

- **C++ Standard**: C++20 or later
- **Compiler Support**: Any modern C++ compiler with C++20 support (MSVC, Clang, GCC)
- **Build System**: CMake 3.21+

### Compiler Support Matrix

| OS / Compiler	| GCC	| Clang	| MSVC	|
| :---			| :---:	| :---:	| :---:	|
| **Linux**		| [![GCC 14](https://github.com/frog-singing/Logicwise/actions/workflows/ci-linux-gcc.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | [![Clang 19](https://github.com/frog-singing/Logicwise/actions/workflows/ci-linux-clang.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) |
| **Windows**	| ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | [![MSVC](https://github.com/frog-singing/Logicwise/actions/workflows/ci-windows-msvc.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) |
| **macOS**		| ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | [![Apple Clang](https://github.com/frog-singing/Logicwise/actions/workflows/ci-macos-clang.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) |

### Header-Only Library

Logicwise is a header-only library — simply include it:

```cpp
#include <logicwise.h>
```

### CMake Integration

Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(logicwise
  GIT_REPOSITORY "https://github.com/frog-singing/Logicwise.git"
  GIT_TAG "v1.0.0"
)

FetchContent_MakeAvailable(logicwise)

target_link_libraries(your_target PRIVATE logicwise::logicwise)
```

### Manual Integration

Download the [Logicwise](https://github.com/frog-singing/Logicwise) library
and include the `Logicwise/library` directory in your project's include paths.

Logicwise also depends on the [Manipond](https://github.com/frog-singing/Manipond) library.
Download Manipond and include the `Manipond/exosuit_library` and `Manipond/meta_library` directories in your project's include paths.

It is recommended to place Manipond at `Logicwise/external/Manipond`, which is the default path specified in `Logicwise/CMakeLists.txt`.

<!-------------------------------------------------------------------------------->

## 📝 Vocabulary List

### [9+ Quantifiers](library/logicwise/mode.h)

#### Standard Quantifier

| Quantifier	| Symbol	| Term									| Meaning				|
| ---			| ---		| ---									| ---					|
| all_of		| ∀			| Universal Quantifier					| For all				|
| any_of		| ∃			| Existential Quantifier				| There exists			|
| none_of		| ¬∃, ∀¬	| Negation of Existential Quantifier	| There does not exist	|
| not_every		| ¬∀, ∃¬	| Negation of Universal Quantifier		| Not for all			|

#### Numerical Quantifier

| Quantifier	| Symbol			| Term							| Meaning						|
| ---			| ---				| ---							| ---							|
| exactly\<N>	| ∃<sub>=N</sub>	| Exact Numerical Quantifier	| Exactly N						|
| at_least\<N>	| ∃<sub>≥N</sub>	| Lower-Bound Quantifier		| Greater than or equal to N	|
| at_most\<N>	| ∃<sub>≤N</sub>	| Upper-Bound Quantifier		| Less than or equal to N		|
| more_than\<N>	| ∃<sub>>N</sub>	| Strict Lower-Bound Quantifier	| Strictly greater than N		|
| less_than\<N>	| ∃<sub><N</sub>	| Strict Upper-Bound Quantifier	| Strictly less than N			|

[**[Prologue Sneak Peek]** Wondering what you can do with these quantifiers?](test/mode/verification/quantifier.cpp)

### [8+ Arrangements](library/logicwise/arrangement.h)

#### Elementwise

| Arrangement	| Meaning				|
| ---			| ---					|
| element		| Individual elements	|

#### Pairwise

| Arrangement				| Meaning								|
| ---						| ---									|
| permutation_pair			| Permutations of any 2 elements		|
| combination_pair			| Order-preserving combination pairs	|
| linear_adjacent_pair		| One-way linear adjacent pairs			|
| circular_adjacent_pair	| One-way circular adjacent pairs		|

#### Bipartite

| Arrangement			| Meaning						|
| ---					| ---							|
| cartesian_pair		| Cartesian product pairs		|
| zip_pair_truncation	| Truncated zip-aligned pairs	|
| zip_pair_padding		| Padded zip-aligned pairs		|

Both quantifier and arrangement are extensible!

<!-------------------------------------------------------------------------------->

## 📦 Container Adaptation

| Container			| Requirements	| Examples	| Counterexamples	|
| ---				| ---			| ---		| ---				|
| **Type Wrapper**	| Class template <br> where all parameters are types				| 1. `wrapper::type_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<bool, int, double, char>` <br> 2. `std::tuple<bool, int, double, char>` | `std::array<int, 4>` <br> <br> _* The second argument is not a type_ |
| **Value Wrapper**	| Class template <br> where all parameters <br> are values (NTTP)	| `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;`<true, 2, 3.0, '4'>` | `std::make_index_sequence<4>` <br> <br> _* This alias actually resolves to: <br> &nbsp;&nbsp;`std::integer_sequence` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<std::size_t, 0, 1, 2, 3>`, <br> &nbsp;&nbsp;where the first argument is not a value_ |
| **Vector-like <br> Container**	| Contiguous memory, <br> amortized O(1) size retrieval, <br> and subscript access | 1. `std::vector<int>{ 1, 2, 3, 4 }` <br> 2. `std::array{ 1, 2, 3, 4 }` <br> 3. `int my_array[4]{ 1, 2, 3, 4 };` <br> 4. `std::span{ my_array }` | `std::list<int>{ 1, 2, 3, 4 }` <br> <br> _* Non-contiguous memory_ |
| **Tuple-like <br> Container**		| Not directly supported; <br> could be converted <br> using `to_variant_array` <br> and accessed via `std::visit` | `container::to_variant_array(` <br> &nbsp;&nbsp;&nbsp;&nbsp;`std::make_tuple(true, 2, 3.0, '4')` <br> `)` | 1. `std::make_tuple(1, 2, 3, 4)` <br> 2. `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<1, 2, 3, 4>{}` <br> <br> _* Direct use of tuple-like containers <br> &nbsp;&nbsp;is not supported_ |

*Note: For bipartite arrangements, in scenarios of a meta container and an instance container,
the static size of the instance container is required.
Thus, the requirement for the instance container is elevated from a Vector-like Container to an **Array-like Container**.*

<!-------------------------------------------------------------------------------->

## 🎭 Logic Theater

Logic does not emerge from the void; it resides within our daily experiences and practical needs.  
The following test cases, structured as a series of "plays", will guide you through those overlooked corners —
embarking on a logical journey from the trivial to the abstract, and ultimately returning to the everyday.

> **Take your seats — the curtain is about to rise!**

### Prologue: Another Encounter with Quantifiers 🏃‍➡️💖🏃‍

#### [Wait, What Quantifier?](test/mode/verification/quantifier.cpp)

> Don't panic! You’ve probably used quantifiers a thousand times in your code without even knowing their formal name.  
> Let's get reacquainted with them and see how they act in some fundamental scenarios.

### Act I: I Am One of Them — Elementwise 😒🤪😑

#### [Finally, I Can See You Crystal Clear](test/mode/verification/elementwise.cpp)

```cpp
rangewise<Quantifier, ElementwiseArrangement>
	::in<Range>()
	.satisfies(predicate);
```

> In the past, once a template parameter entered a template,
> it was like falling into a black hole, completely losing its trace.   
> But now, not only can you see them, you can scrutinize them!

### Act II: Relations Compose the Whole — Pairwise 😊🤝😊

#### [The Best-Laid Plans of Seasons Often Go Awry](test/mode/verification/pairwise.cpp)

```cpp
rangewise<Quantifier, PairwiseArrangement>
	::in<Range>()
	.satisfies(predicate);
```

> Spring, summer, autumn, and winter.
> Seasons change exactly as they should, but sticking to plans is never quite so regular.  
> How do our seasonal goals look by the end of the year?
> And if plans do change, can we still remain true to our original aspirations?

### Act III: It Takes Two — Bipartite ✊✋✌️

#### [Meta Verification: A Worthy Opponent](test/mode/verification/bipartite_meta.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between<RangeA, RangeB>()
	.satisfies(predicate);
```

> The battle gets intense. How do we break the stalemate? Let's see what classic game theory can teach us.

#### [Instance Verification: Dungeon & Adventurer](test/mode/verification/bipartite_instance.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between(rangeA, rangeB)
	.satisfies(predicate);
```

> The adventurer party sets out on their journey.
> The dungeon holds mystical treasures, but danger also lurks at every turn.
> Sometimes their path is smooth sailing; other times, they are beset on all sides.  
> Unknown depths lie ahead for them to explore, where the next foe could be a monster, or perhaps, other adventurers.

#### [Meta Meets Instance: Tom's Fruit List](test/mode/verification/bipartite_meta_and_instance.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between<RangeA>(rangeB)
	.satisfies(predicate);
```

> Tom has a fruit list. One day, he feels like grabbing some fruit to eat, only to find himself sharing with...

<!-------------------------------------------------------------------------------->

## 🏭 Logic Factory

Logicwise goes beyond verification — it can also be used to forge new C++ concepts!  
Just feed in some existing concepts as raw materials, choose the right quantifier and arrangement,
and the Logicwise assembly line will manufacture a brand-new concept for you!

> **Clunk, Clunk!** (Sounds of factory machinery)

### [Wrapper](library/logicwise/wrapper.h)

Here, a wrapper refers to a class template that bundles a group of pure types or pure values into a single entity —
essentially, a meta container.  
Utilizing Logicwise's syntax, you can easily construct various practical wrappers.
Logicwise provides several preset wrappers out of the box, such as sets, posets (partially ordered sets), and antichains.  
These wrappers do more than just replace the basic `type_list` and `value_list`;
they can be plugged directly into the range slot of the `rangewise` syntax to enable increasingly sophisticated logical modeling.

If you want to build your own wrapper, take a look at the example below.  
First, define a concept for your wrapper:

```cpp
template<typename Range>
concept Set =
	rangewise<none_of, combination_pair>
	::template in<Range>()
	.satisfies([] <typename T1, typename T2> { return std::same_as<T1, T2>; });
```

Then, use this concept to constrain your wrapper:

```cpp
template<typename... Type>
	requires Set<type_list<Type...>>
struct set {};
```

Of course, if you prefer not to split them up,
writing the full `rangewise` syntax directly within the wrapper's definition works perfectly too:

```cpp
template<typename... Type>
	requires (
		rangewise<none_of, combination_pair>
		::template in<type_list<Type...>>()
		.satisfies([] <typename T1, typename T2> { return std::same_as<T1, T2>; })
	)
struct set {};
```

Next, try putting your newly defined wrapper to work.
It can be passed straight into the range slot of the `rangewise` syntax:

```cpp
template<typename... Integral>
concept DistinctRawIntegrals =
	rangewise<all_of, element>
	::in<set<Integral...>>()
	.satisfies([] <typename T> { return std::integral<T>; });
```

If you are curious about how Logicwise implements wrappers under the hood, check out the
[type set](library/logicwise/wrapper/set/type_set.h).
The example above is a simplified version of it.

### [Relation](library/logicwise/relation.h)

Here, a relation is defined specifically over types or values,
typically appearing as a binary predicate in the form of a concept or a Lambda expression.  
Utilizing Logicwise's syntax, you can take fundamental relations as raw materials to forge complex relations.
Logicwise provides several preset relations out of the box, such as inclusion relations and exclusion relations.  
From there, you can feed these relations right back into the predicate slot of the `rangewise` syntax
to construct even higher-level relations.

If you want to build your own relations, take a look at the example below.  
First, take the fundamental relation `std::same_as` as raw material to construct a `BelongsTo` relation:

```cpp
template<typename Type, typename Range>
concept BelongsTo =
	rangewise<at_least<1>, element>
	::template in<Range>()
	.satisfies([] <typename Element> { return std::same_as<Type, Element>; });
```

Then, you can use this `BelongsTo` relation as raw material to construct a `SubRangeOf` relation:

```cpp
template<typename SubRange, typename SuperRange>
concept SubRangeOf =
	rangewise<all_of, element>
	::template in<SubRange>()
	.satisfies([] <typename Element> { return BelongsTo<Element, SuperRange>; });
```

Finally, combine the `Set` constraint from the previous example with the `SubRangeOf` relation here,
and you assemble a mathematically flawless `SubSetOf` relation:

```cpp
template<typename SubSet, typename SuperSet>
concept SubSetOf =
	Set<SubSet> && Set<SuperSet> &&
	SubRangeOf<SubSet, SuperSet>;
```

If you are curious about how Logicwise implements relations under the hood, check out the
[typewise inclusion relation](library/logicwise/relation/inclusion/type_inclusion.h).
The example above is a simplified version of it.

<!-------------------------------------------------------------------------------->

## 🌀 Logic Limbo

This is the limbo of logic.  
From here, you will delve into the logical structures, internal mechanisms, consistency verification,
and degenerate cases, step by step approaching the brink of logical collapse.

> **If you have stumbled into this place, this is your last chance to turn back.**

### Arrangement Behavior

Verify the correctness of index sequence for each arrangement in both general and degenerate cases.

#### [Elementwise](test/arrangement/elementwise.cpp)

> | Arrangement	| Meaning				|
> | ---			| ---					|
> | element		| Individual elements	|

#### [Pairwise](test/arrangement/pairwise.cpp)

> | Arrangement				| Meaning								|
> | ---						| ---									|
> | permutation_pair		| Permutations of any 2 elements		|
> | combination_pair		| Order-preserving combination pairs	|
> | linear_adjacent_pair	| One-way linear adjacent pairs			|
> | circular_adjacent_pair	| One-way circular adjacent pairs		|

#### [Bipartite](test/arrangement/bipartite.cpp)

> | Arrangement			| Meaning						|
> | ---					| ---							|
> | cartesian_pair		| Cartesian product pairs		|
> | zip_pair_truncation	| Truncated zip-aligned pairs	|

#### [Bipartite with Padding](test/arrangement/bipartite_padding.cpp)

> | Arrangement			| Meaning						|
> | ---					| ---							|
> | zip_pair_padding	| Padded zip-aligned pairs		|

### Quantifier Behavior

#### [Short-Circuit Evaluation](test/behavior/quantifier/short_circuit_evaluation.cpp)

> Quantifiers support short-circuit evaluation, meaning that once the result is determined,
> no further predicate evaluation is performed on the subsequent sequence.  
> Note that a determined result does not imply that the quantifier is satisfied.

#### [De Morgan's Laws](test/behavior/quantifier/de_morgan_law.cpp)

> De Morgan's First Law: The negation of a conjunction is equivalent to the disjunction of the negations.  
> ¬(P1 ∧ P2 ∧ ... ∧ Pn) ≡ (¬P1) ∨ (¬P2) ∨ ... ∨ (¬Pn)
>
> De Morgan's Second Law: The negation of a disjunction is equivalent to the conjunction of the negations.  
> ¬(P1 ∨ P2 ∨ ... ∨ Pn) ≡ (¬P1) ∧ (¬P2) ∧ ... ∧ (¬Pn)
>
> Used to verify the correctness of `all_of`, `any_of`, `none_of`, and `not_every` behaviors.

### Degeneracy

#### [CV Ref Ptr Matrix](test/degeneracy/cv_ref_ptr_matrix.cpp)

> In template parameter verification scenarios involving `const` and `volatile` qualifiers,
> as well as reference and pointer declarators, the identity of template parameters may deviate unexpectedly.  
> This test verifies Logicwise's correctness in handling these edge cases
> by constructing matrices of various qualifier and declarator combinations.

#### [Non-Traversable Extent](test/degeneracy/non_traversable_extent.cpp)

> For scenarios involving a meta container, when the extent is non-traversable,
> since there are no single variables or variable groups that need to be verified by the predicate,
> it is impossible to guarantee that valid meta elements — whether types or values — can be retrieved as probes.  
> In such cases, no constraints are applied on the shape of the predicate,
> specifically the number and properties of its parameters.  
> That is, the empty set can match predicates of any shape.

<!-------------------------------------------------------------------------------->

## 🎬 Behind the Scenes

### Important Notes

In principle, the contents within `namespace logicwise::detail` are intended for internal use only
and their stability is not guaranteed.
Unless you know exactly what you are doing, please do not use or depend on them directly.  
You can use other contents within `namespace logicwise` as long as they are not marked as `[unimplemented]`.

### Side Effects

For now, Logicwise is targeted just for verification, so don't ever stuff side effects into the callback functions!
And also don't treat it as a traversal framework...

Well, I know the following code compiles:

```cpp
using my_list = wrapper::type_list<bool, int, double, char>;

constexpr int count = []
	{
		int i{ 0 };

		(void)rangewise<quantifier::all_of, arrangement::element>
			::in<my_list>()
			.satisfies([&] <typename T> { ++i; return true; });

		return i;
	}();

static_assert(count == 4, "");
```

But that's way too weird! **The verification mode does not guarantee a uniform traversal direction.**
Just promise me you won't write code like this, alright?  
~~(If you guys really need it, maybe I'll consider adding a traversal mode...)~~  
(The traversal mode is available now, but I haven't had the time to test it yet.)

### Known Compiler Issues

For detailed information about compiler compatibility and known issues, see [KNOWN_COMPILER_ISSUES](KNOWN_COMPILER_ISSUES.md).

### To Be Done

Although I have said a lot about wrappers and relations, I didn't get around to testing them yet.
Actually, they belong to a higher-level system, and some of the dependency features haven't been implemented.
For now, let's just treat them as bonus features.

The performance test remains a quest for another day. Right now, correctness is still the main focus.

As for the features labeled `[unimplemented]` — don't take them seriously. I just wrote them for fun.

<!-------------------------------------------------------------------------------->

## 📜 License

```text
MIT License
Copyright © 2026 Frog Singing (@frog-singing)
```

See the [LICENSE](LICENSE) file for details.

<!-------------------------------------------------------------------------------->

## 💡 Hello Logic!

```cpp
std::cout << "Hello Logic!" << std::endl;
```

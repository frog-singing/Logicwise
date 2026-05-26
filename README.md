<div align="center">

# Logicwise

*Factory for C++ Concepts*

English | [简体中文](README.zh-Hans.md)

</div>

<!-------------------------------------------------------------------------------->

## 🔍 Grammar Overview

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

*Validate as you declare. Let the code speak for itself.*

</div>

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

[**[Prologue Sneak Peek]** Wondering what you can do with these quantifiers?](test/mode/validation/quantifier.cpp)

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
| **Type List**		| Class template instantiations <br> where all arguments are types			| 1. `wrapper::type_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<bool, int, double, char>` <br> 2. `std::tuple<bool, int, double, char>` | `std::array<int, 4>` <br> <br> _* The second argument is not a type_ |
| **Value List**	| Class template instantiations <br> where all arguments <br> are values (NTTP)	| `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;`<true, 2, 3.0, '4'>` | `std::make_index_sequence<4>` <br> <br> _* This alias actually resolves to: <br> &nbsp;&nbsp;`std::integer_sequence` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<std::size_t, 0, 1, 2, 3>`, <br> &nbsp;&nbsp;where the first argument is not a value_ |
| **Vector-like <br> Container**	| Contiguous memory, <br> amortized O(1) size retrieval, <br> and subscript access | 1. `std::vector<int>{ 1, 2, 3, 4 }` <br> 2. `std::array{ 1, 2, 3, 4 }` <br> 3. `int my_array[4]{ 1, 2, 3, 4 };` <br> 4. `std::span{ my_array }` | `std::list<int>{ 1, 2, 3, 4 }` <br> <br> _* Non-contiguous memory_ |
| **Tuple-like <br> Container**		| Not directly supported; <br> could be converted <br> using `to_variant_array` <br> and accessed via `std::visit` | `container::to_variant_array(` <br> &nbsp;&nbsp;&nbsp;&nbsp;`std::make_tuple(true, 2, 3.0, '4')` <br> `)` | 1. `std::make_tuple(1, 2, 3, 4)` <br> 2. `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<1, 2, 3, 4>{}` <br> <br> _* Direct use of tuple-like containers <br> &nbsp;&nbsp;is not supported_ |

*Note: For bipartite arrangements, in scenarios of a meta container and an instance container,
the static size of the instance container is required.
Thus, the requirement for the instance container is elevated from a Vector-like Container to an* ___Array-like Container___*.*

<!-------------------------------------------------------------------------------->

## 🎭 Logic Theater

Logic does not emerge from the void; it resides within our daily experiences and practical needs.  
The following test cases, structured as a series of "plays", will guide you through those overlooked corners —
embarking on a logical journey from the trivial to the abstract, and ultimately returning to the everyday.

> **Take your seats — the curtain is about to rise!**

### Prologue: Another Encounter with Quantifiers 🏃‍➡️💖🏃‍

#### [Wait, What Quantifier?](test/mode/validation/quantifier.cpp)

> Don't panic! You’ve probably used quantifiers a thousand times in your code without even knowing their formal name.  
> Let's get reacquainted with them and see how they act in some fundamental scenarios.

### Chapter I: I am One of Them — Elementwise 😒🤪😑

#### [Finally, I Can See You Crystal Clear](test/mode/validation/elementwise.cpp)

> In the past, once a template parameter entered a template,
> it was like falling into a black hole, completely losing its trace.   
> But now, not only can you see them, you can scrutinize them!

### Chapter II: Relations compose the whole — Pairwise

#### [Placeholder](test/mode/validation/pairwise.cpp)

> sample text

### Chapter III: Bipartite

#### [Section 1: Meta](test/mode/validation/bipartite_meta.cpp)

> sample text

#### [Section 2: Instance](test/mode/validation/bipartite_instance.cpp)

> sample text

#### [Section 3: Meta Meets Instance](test/mode/validation/bipartite_meta_and_instance.cpp)

> sample text

<!-------------------------------------------------------------------------------->

## 🤔 Design Philosophy

说人话：通过语法直接对逻辑建模，只需要你一点点的数学思维

三界统一：类型、常量、实例

零成本抽象

- **Declarative Modeling (Let's Speak Human)**:
We believe logic shouldn't be buried in a labyrinth of template meta-programming.
Logicwise allows you to model predicate logic directly through highly intuitive syntax,
requiring only a fraction of mathematical intuition.

- **The Great Unification**:
One grammar to rule them all. Uniformly audit and verify across three distinct domains:
compile-time types (Meta), compile-time constants (Value), and runtime containers (Instance).

- **Zero-Cost Abstraction**:
Heavy-duty logic compiled down to zero runtime overhead.
The complex multidimensional folding collapses entirely at compile-time,
leaving your runtime binary clean, efficient, and direct.

<!-------------------------------------------------------------------------------->

## 🏭 Logic Factory

Logicwise is not just a validation engine; it is a factory for formal algebraic structures.
Located in the `library/` directory are our high-order, pre-verified logic components:

### [Wrapper](library/logicwise/wrapper.h)

Provides formal mathematical sets like **Poset (Partially Ordered Set)**, **Chain (Total Order)**, **Antichain (Incomparable Set)**, and **Equivalence Class** as first-class, composable C++ types.

### [Relation](library/logicwise/relation.h)

Houses automatic algebraic induction machinery
(e.g., generating strict order, dual order, and incomparability out of a single user-defined partial relation).

<!-------------------------------------------------------------------------------->

## 🌀 Logic Limbo

### Arrangement Behavior

验证 1D 到 2D 拓扑空间的索引生成纯度与对齐安全性

#### [Elementwise](test/arrangement/elementwise.cpp)

> sample text

#### [Pairwise](test/arrangement/pairwise.cpp)

> sample text

#### [Bipartite](test/arrangement/bipartite.cpp)

> sample text

#### [Bipartite with Padding](test/arrangement/bipartite_padding.cpp)

> sample text

### Quantifier Behavior

#### [Short Circuit Evaluation](test/behavior/quantifier/short_circuit_evaluation.cpp)

> sample text

#### [De Morgan's laws](test/behavior/quantifier/de_morgan_law.cpp)

> sample text

### Degeneracy

#### [CV Ref Ptr Matrix](test/degeneracy/cv_ref_ptr_matrix.cpp)

> sample text

#### [Non-traversable Extent](test/degeneracy/non_traversable_extent.cpp)

> sample text

<!-------------------------------------------------------------------------------->

## 🎬 Behind the Scenes

- **Wrapper**: Not tested yet
- **Relation**: Not tested yet

### Side Effects

For now, Logicwise is targeted just for validation, so don't ever stuff side effects into the callback functions!
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

But that's way too weird! **The validation mode does not guarantee a uniform traversal direction.**
Just promise me you won't write code like this, alright?
(If you guys really need it, maybe I'll consider adding a traversal mode...)

### Known Compiler Issues

For detailed information about compiler compatibility and known issues, see [KNOWN_COMPILER_ISSUES](KNOWN_COMPILER_ISSUES.md).

<!-------------------------------------------------------------------------------->

## 🔌 Integration

### Requirements

- **C++ Standard**: C++20 or later
- **Compiler Support**: Any modern C++ compiler with C++20 support (MSVC, GCC, Clang)
- **Build System**: CMake 3.21+

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
  GIT_REPOSITORY https://github.com/frog-singing/Logicwise.git
  GIT_TAG main
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

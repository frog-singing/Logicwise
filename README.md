<div align="center">

# Logicwise

*Factory for C++ Concepts*

English | [简体中文](README.zh-Hans.md)

</div>

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
<td align="center">
<strong style="font-size: 1.5em;">
Quantifier × Arrangement × Range × Predicate
</strong>
</td>
</tr>
</table>

Validate as you declare. Let the code speak for itself.

</div>

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

| Arrangement	| Meaning			|
| ---			| ---				|
| element		| Single elements	|

#### Pairwise

| Arrangement				| Meaning								|
| ---						| ---									|
| permutation_pair			| Full permutation pairs				|
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

## 📦 Container Adaptation

| Container				| Requirements	| Examples	| Eounterexamples	|
| ---					| ---			| ---		| ---				|
| Type List				|	| type_list<bool, int, double, char> <br> std::tuple<bool, int, double, char> |		|
| Value List			|	| value_list<true, 2, 3.0, '4'> |		|
| Vector-like Container	|	| std\::vector\<int>{1, 2, 3, 4} <br> int[4]{1, 2, 3, 4} <br> std::span\{ my_array } |		|
| Tuple-like Container	|	| container\::to_variant_array( <br> &nbsp;&nbsp;&nbsp;&nbsp;std::make_tuple(true, 2, 3.0, '4') <br> ) |		|

注：在二部的元容器和实体容器场景中，需要获取实体容器的静态大小，即对实体容器的要求从类向量容器升级为类数组容器

## 🎭 Logic Theater

### Prologue: Another Encounter with Quantifiers 🏃‍➡️💖🏃‍

#### [Wait, What Quantifier?](test/mode/validation/quantifier.cpp)

Don't panic! You’ve probably used quantifiers a thousand times in your code without even knowing their formal name.  
Let's get reacquainted with them and see how they act in some fundamental scenarios.

### [Chapter I: Elementwise](test/mode/validation/elementwise.cpp)

### [Chapter II: Pairwise](test/mode/validation/pairwise.cpp)

### Chapter III: Bipartite

#### [Section 1: Meta](test/mode/validation/bipartite_meta.cpp)

#### [Section 2: Instance](test/mode/validation/bipartite_instance.cpp)

#### [Section 3: Meta Meets Instance](test/mode/validation/bipartite_meta_and_instance.cpp)

## 🤔 Design Philosophy

## 🌀 Logic Limbo

### Arrangement Behavior

#### [Elementwise](test/arrangement/elementwise.cpp)

#### [Pairwise](test/arrangement/pairwise.cpp)

#### [Bipartite](test/arrangement/bipartite.cpp)

#### [Bipartite with Padding](test/arrangement/bipartite_padding.cpp)

### Quantifier Behavior

#### [Short Circuit Evaluation](test/behavior/quantifier/short_circuit_evaluation.cpp)

#### [De Morgan's laws](test/behavior/quantifier/de_morgan_law.cpp)

### Degeneracy

#### [CV Ref Ptr Matrix](test/degeneracy/cv_ref_ptr_matrix.cpp)

#### [Non-traversable Extent](test/degeneracy/non_traversable_extent.cpp)

## 🎬 Behind the Scenes

## 🔌 Integration

## 📜 License

## 💡 Hello Logic!

```cpp
std::cout << "Hello Logic!" << std::endl;
```

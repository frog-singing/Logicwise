<div align="center">

# Logicwise

*一个 C++ 概念工厂*

[English](README.md) | 简体中文

</div>

<!-------------------------------------------------------------------------------->

## 🔍 语法速览

```cpp
rangewise<all_of, cartesian_pair>
	::between<RangeA, RangeB>()
	.satisfies(predicate);
```

翻译如下：

> 按范围维度，范围 A 和范围 B 之间的所有笛卡尔积对都满足给定谓词 P。
> 
> ∀x ∈ A, ∀y ∈ B, P(x, y)  

或者：

> ∀(x, y) ∈ A×B, P(x, y)

<div align="center">

以正交方式构造你的逻辑：

<table>
	<tr>
	    <td style="padding: 14px 28px;" align="center">
			<strong style="font-size: 1.5em">
				量词 × 排布 × 范围 × 谓词
			</strong>
		</td>
	</tr>
</table>

*声明式验证，让代码说人话。*

</div>

<!-------------------------------------------------------------------------------->

## 🤔 设计哲学

- **说人话**：通过自然的语法直接对逻辑建模，只需要你一点点的数学思维。

- **正交架构**：量词、排布、范围和谓词相互独立，任意组合，灵活扩展。

- **语法统一**：只需要学习一种语法，就能丝滑验证类型、值和实例。

- **物种杂交**：在二部验证中，类型容器、值容器和实例容器之间的混合匹配毫无压力，打破 C++ 的物种隔离。

- **零成本抽象**：纯编译期逻辑验证直接坍缩为布尔常量，不引入任何运行时开销。同时在编译期，也尽量从理论上避免额外的编译开销。

<!-------------------------------------------------------------------------------->

## 📖 快速入门

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

	// 验证模式 --------------------------------------------------------------------------------

	static_assert(
		rangewise<none_of, element>
		::in<type_list<double, void, int&, type_list<int>, double>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"按范围维度，类型列表中不存在元素满足“和 int 是相同类型”。"
	);

	static_assert(
		rangewise<any_of, combination_pair>
		::in<value_list<1, 2, 3, 4>>()
		.satisfies([] <auto V1, auto V2> { return (V1 + V2) == 5; }),
		"按范围维度，值列表中存在组合对满足“两个值的和等于 5”。"
	);

	static_assert(
		rangewise<exactly<9>, cartesian_pair>
		::between<value_list<true, 2, 3.0>>(std::array{ 1, 2, 3, 4 })
		.satisfies([] <auto V1>(auto&& v2) { return V1 != v2; }),
		"按范围维度，值列表和数组之间恰好有 9 个笛卡尔积对满足“两个值不相等”。"
	);

	// 遍历模式 --------------------------------------------------------------------------------

	constexpr int count_1 = []
		{
			int i{ 0 };

			rangewise<for_each, element>
				::in(std::array{ 1, 2, 3, 4 })
				.execute([&] (auto&& v) { if (v % 2 == 0) { ++i; } });

			return i;
		}();

	static_assert(count_1 == 2, "按范围维度，数组中有 2 个偶数。");

	int count_2{ 0 };

	rangewise<for_each, linear_adjacent_pair>
		::in<type_list<bool, int, double, double, char>>()
		.execute_until([&] <typename T1, typename T2> {
			if constexpr (std::same_as<T1, T2>) { return true; }
			++count_2;
			std::cout << "<" << typeid(T1).name() << ", " << typeid(T2).name() << ">" << std::endl;
			return false;
		});

	std::cout << "计数：" << count_2 << std::endl << std::endl;

	/*
	输出：
	<bool, int>
	<int, double>
	计数：2
	*/

	int count_3{ 0 };

	rangewise<in_reverse_for_each, zip_pair_truncation>
		::between<type_list<bool, int, double, char, void>, value_list<true, 2u, 3.0, 4>>()
		.execute([&] <typename T, auto V> {
			if constexpr (!std::same_as<T, decltype(V)>) { return; }
			++count_3;
			std::cout << "<" << typeid(T).name() << ", " << V << ">" << std::endl;
		});

	std::cout << "计数：" << count_3 << std::endl << std::endl;
	
	/*
	输出：
	<double, 3>
	<bool, 1>
	计数：2
	*/

	std::cout << "Hello Logic!" << std::endl;
}
```

<!-------------------------------------------------------------------------------->

## 🔌 集成方式

### 开发环境要求

- **C++ 标准**：C++20 或更高版本
- **编译器支持**：任何支持 C\++20 的现代 C++ 编译器（MSVC、Clang、GCC）
- **构建系统**：CMake 3.21+

### 编译器支持矩阵

| 操作系统 \ 编译器	| GCC	| Clang	| MSVC	|
| :---				| :---:	| :---:	| :---:	|
| **Linux**		| [![GCC 14](https://github.com/frog-singing/Logicwise/actions/workflows/ci-linux-gcc.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | [![Clang 19](https://github.com/frog-singing/Logicwise/actions/workflows/ci-linux-clang.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) |
| **Windows**	| ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | [![MSVC](https://github.com/frog-singing/Logicwise/actions/workflows/ci-windows-msvc.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) |
| **macOS**		| ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) | [![Apple Clang](https://github.com/frog-singing/Logicwise/actions/workflows/ci-macos-clang.yml/badge.svg?branch=main)](https://github.com/frog-singing/Logicwise/actions) | ![N/A](https://img.shields.io/badge/OS--Mismatch-grey?style=flat-square) |

### 仅头文件库

Logicwise 是一个仅头文件库，直接包含即可：

```cpp
#include <logicwise.h>
```

### CMake 集成

在你的 `CMakeLists.txt` 中添加：

```cmake
include(FetchContent)

FetchContent_Declare(logicwise
  GIT_REPOSITORY "https://github.com/frog-singing/Logicwise.git"
  GIT_TAG "v1.0.0"
)

FetchContent_MakeAvailable(logicwise)

target_link_libraries(your_target PRIVATE logicwise::logicwise)
```

### 手动集成

下载 [Logicwise](https://github.com/frog-singing/Logicwise) 库，将 `Logicwise/library` 目录包含在你项目的包含路径中。

Logicwise 同时依赖了 [Manipond](https://github.com/frog-singing/Manipond) 库，因此你还需要下载 Manipond 库，然后将 `Manipond/exosuit_library` 和 `Manipond/meta_library` 目录包含在你项目的包含路径中。

推荐把 Manipond 放到 `Logicwise/external/Manipond` ，这是 `Logicwise/CMakeLists.txt` 中的预设路径。

<!-------------------------------------------------------------------------------->

## 📝 词汇表

### [9+ 个量词](library/logicwise/mode.h)

#### 标准量词

| 量词		| 符号		| 术语			| 含义		|
| ---		| ---		| ---			| ---		|
| all_of	| ∀			| 全称量词		| 所有		|
| any_of	| ∃			| 存在量词		| 存在		|
| none_of	| ¬∃, ∀¬	| 存在否定量词	| 不存在		|
| not_every	| ¬∀, ∃¬	| 全称否定量词	| 并非所有	|

#### 数量量词

| 量词			| 符号				| 术语			| 含义			|
| ---			| ---				| ---			| ---			|
| exactly\<N>	| ∃<sub>=N</sub>	| 精确数量量词	| 恰好 N 个		|
| at_least\<N>	| ∃<sub>≥N</sub>	| 下界量词		| 大于等于 N 个	|
| at_most\<N>	| ∃<sub>≤N</sub>	| 上界量词		| 小于等于 N 个	|
| more_than\<N>	| ∃<sub>>N</sub>	| 严格下界量词	| 严格大于 N 个	|
| less_than\<N>	| ∃<sub><N</sub>	| 严格上界量词	| 严格小于 N 个	|

[【序章偷跑】想不想看看你能用这些量词做点什么？](test/mode/verification/quantifier.cpp)

### [8+ 种排布](library/logicwise/arrangement.h)

#### 逐元素

| 排布		| 含义		|
| ---		| ---		|
| element	| 单个元素	|

#### 逐对

| 排布						| 含义			|
| ---						| ---			|
| permutation_pair			| 全排列对		|
| combination_pair			| 保序组合对		|
| linear_adjacent_pair		| 单向线性相邻对	|
| circular_adjacent_pair	| 单向环状相邻对	|

#### 二部

| 排布					| 含义		|
| ---					| ---		|
| cartesian_pair		| 笛卡尔积对	|
| zip_pair_truncation	| 截断对齐对	|
| zip_pair_padding		| 填充对齐对	|

量词和排布都支持扩展！

<!-------------------------------------------------------------------------------->

## 📦 容器适配

| 容器			| 要求								| 举例		| 反例		|
| ---			| ---								| ---		| ---		|
| **类型包装器**	| 所有参数都是类型的 <br> 类模板		| 1. `wrapper::type_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<bool, int, double, char>` <br> 2. `std::tuple<bool, int, double, char>` | `std::array<int, 4>` <br> <br> _* 第二位参数并非类型_ |
| **值包装器**	| 所有参数都是值（NTTP）的 <br> 类模板	| `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;`<true, 2, 3.0, '4'>` | `std::make_index_sequence<4>` <br> <br> _* 该别名的实际类型为： <br> &nbsp;&nbsp;`std::integer_sequence` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<std::size_t, 0, 1, 2, 3>`， <br> &nbsp;&nbsp;第一位参数并非值_ |
| **类向量容器**	| 内存连续， <br> 均摊 O(1) 时间获取大小， <br> 支持下标访问 | 1. `std::vector<int>{ 1, 2, 3, 4 }` <br> 2. `std::array{ 1, 2, 3, 4 }` <br> 3. `int my_array[4]{ 1, 2, 3, 4 };` <br> 4. `std::span{ my_array }` | `std::list<int>{ 1, 2, 3, 4 }` <br> <br> _* 内存不连续_ |
| **类元组容器**	| 不直接支持类元组容器； <br> 提供了 `to_variant_array` <br> 方法来转换， <br> 并需要配合 `std::visit` 访问	| `container::to_variant_array(` <br> &nbsp;&nbsp;&nbsp;&nbsp;`std::make_tuple(true, 2, 3.0, '4')` <br> `)` | 1. `std::make_tuple(1, 2, 3, 4)` <br> 2. `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<1, 2, 3, 4>{}` <br> <br> _* 不支持直接传入类元组容器_ |

*注：在二部排布的元容器和实例容器场景中，需要获取实例容器的静态大小，因此对实例容器的要求从类向量容器提升为**类数组容器**。*

<!-------------------------------------------------------------------------------->

## 🎭 逻辑小剧场

逻辑并不诞生自虚空，而是藏身于我们的日常体验和实际需求中。  
以下测试用例将以“剧目”的形式，带你走过那些不曾留意的角落，踏上一场从简单到抽象，最终回归日常的逻辑之旅。

> **坐好，演出马上开始！**

### 序幕：你与量词的再次邂逅 🏃‍➡️💖🏃‍

#### [等等，啥是量词？](test/mode/verification/quantifier.cpp)

> 别慌！你一定已经在代码中使用过各种量词了，只是还不知道它们的大名而已。  
> 让我们重新认识一下它们，梳理一下基本用法。

### 第一幕：我是其中之一&#8202;——逐元素 😒🤪😑

#### [借我，借我一双慧眼吧](test/mode/verification/elementwise.cpp)

```cpp
rangewise<Quantifier, ElementwiseArrangement>
	::in<Range>()
	.satisfies(predicate);
```

> 以往，模板参数一旦进入模板，就好像掉入了黑洞，彻底销声匿迹。  
> 然而现在，你不仅能看见它们，还能审视它们！

### 第二幕：关系构成整体——逐对 😊🤝😊

#### [季划赶不上变化](test/mode/verification/pairwise.cpp)

```cpp
rangewise<Quantifier, PairwiseArrangement>
	::in<Range>()
	.satisfies(predicate);
```

> 春去秋来，周而复始。季节更替按部就班，计划执行却规律难循。  
> 年初的计划，到年末会如何？若计划有变，又能否不忘初心？

### 第三幕：孤掌难鸣——二部 ✊✌️✋

#### [元验证：旗鼓相当的对手](test/mode/verification/bipartite_meta.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between<RangeA, RangeB>()
	.satisfies(predicate);
```

> 战况焦灼，如何破局？从经典的博弈论中，让我们看看能学到怎样的智慧。

#### [实例验证：地下城与冒险者](test/mode/verification/bipartite_instance.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between(rangeA, rangeB)
	.satisfies(predicate);
```

> 冒险者小队踏上了旅程。地下城有神秘宝藏，却也危机四伏。旅途有时一帆风顺，有时也腹背受敌。  
> 前方的未知等待着他们探索，下一个敌人是怪物...还是其他冒险者？

#### [当元遇见实例：汤姆的水果清单](test/mode/verification/bipartite_meta_and_instance.cpp)

```cpp
rangewise<Quantifier, BipartiteArrangement>
	::between<RangeA>(rangeB)
	.satisfies(predicate);
```

> 汤姆有个水果清单。一天，他想找点水果吃，不过和他一起分享水果的是...

<!-------------------------------------------------------------------------------->

## 🏭 逻辑工厂

Logicwise 不光能用来验证，还能用来构造新的 C++ 概念！  
你只需要投入一些概念作为原材料，选择合适的量词和排布，Logicwise 流水线就能帮你生产出一个全新的概念！

> **哐当，哐当！**（工厂机器运转声）

### [包装器](library/logicwise/wrapper.h)

这里，包装器指的是可以将一组纯类型或纯值打包成一个整体的类模板，也就是元容器。  
利用 Logicwise 的语法，可以构造出各种实用的包装器。Logicwise 中已经提供了一些预设的包装器，例如集合、偏序集、反链。  
包装器不仅可以替代基础的 `type_list` 和 `value_list`，还可以直接用于 `rangewise` 语法的范围槽位，实现更复杂的逻辑建模。

如果你想构建自己的包装器，不妨看看下面这个示例。  
首先，为你的包装器定义一个概念：

```cpp
template<typename Range>
concept Set =
	rangewise<none_of, combination_pair>
	::template in<Range>()
	.satisfies([] <typename T1, typename T2> { return std::same_as<T1, T2>; });
```

然后，用这个概念来约束你的包装器：

```cpp
template<typename... Type>
	requires Set<type_list<Type...>>
struct set {};
```

当然，如果你不喜欢分开的写法，在包装器定义处直接写出完整的 `rangewise` 语法也是完全可以的：

```cpp
template<typename... Type>
	requires (
		rangewise<none_of, combination_pair>
		::template in<type_list<Type...>>()
		.satisfies([] <typename T1, typename T2> { return std::same_as<T1, T2>; })
	)
struct set {};
```

接着，试着使用一下你刚刚定义好的包装器吧。它可以直接用在 `rangewise` 语法的范围槽位：

```cpp
template<typename... Integral>
concept DistinctRawIntegrals =
	rangewise<all_of, element>
	::in<set<Integral...>>()
	.satisfies([] <typename T> { return std::integral<T>; });
```

如果你对 Logicwise 包装器的底层实现感兴趣，不妨看看[类型集合](library/logicwise/wrapper/set/type_set.h)。上面的示例就是它的精简版。

### [关系](library/logicwise/relation.h)

这里，关系指的是定义在类型或值上的关系，一般表现为一个二元谓词，以概念或者 Lambda 表达式的形式存在。  
利用 Logicwise 的语法，可以用基础关系作为原料，构造出各种复杂的关系。Logicwise 中已经提供了一些预设的关系，例如包含关系、互斥关系。  
然后，你可以继续把这些关系填入 `rangewise` 语法的谓词槽位，构造出更高层级的关系。

如果你想构建自己的关系，不妨看看下面这个示例。  
首先，用 `std::same_as` 这个基础关系作为原料，构造出一个 `属于` 关系：

```cpp
template<typename Type, typename Range>
concept BelongsTo =
	rangewise<at_least<1>, element>
	::template in<Range>()
	.satisfies([] <typename Element> { return std::same_as<Type, Element>; });
```

然后，用 `属于` 关系作为原料，就能构造出一个 `子范围` 关系：

```cpp
template<typename SubRange, typename SuperRange>
concept SubRangeOf =
	rangewise<all_of, element>
	::template in<SubRange>()
	.satisfies([] <typename Element> { return BelongsTo<Element, SuperRange>; });
```

最后，把上一个例子中的 `集合` 约束和这里的 `子范围` 关系组合起来，你就装配出了一个数学上无懈可击的 `子集` 关系：

```cpp
template<typename SubSet, typename SuperSet>
concept SubSetOf =
	Set<SubSet> && Set<SuperSet> &&
	SubRangeOf<SubSet, SuperSet>;
```

如果你对 Logicwise 关系的底层实现感兴趣，不妨看看[类型维度包含关系](library/logicwise/relation/inclusion/type_inclusion.h)。上面的示例就是它的精简版。

<!-------------------------------------------------------------------------------->

## 🌀 逻辑边境

这里是逻辑的过渡地带。  
从这里开始，你将深入逻辑结构、内部机制、一致性验证、退化情形，一步步来到逻辑即将崩塌的边缘。

> **如果你误入此地，这是你最后的回头机会。**

### 排布行为

验证每种排布的索引序列在一般情况和退化情况的正确性。

#### [逐元素](test/arrangement/elementwise.cpp)

> | 排布		| 含义		|
> | ---		| ---		|
> | element	| 单个元素	|

#### [逐对](test/arrangement/pairwise.cpp)

> | 排布						| 含义			|
> | ---						| ---			|
> | permutation_pair		| 全排列对		|
> | combination_pair		| 保序组合对		|
> | linear_adjacent_pair	| 单向线性相邻对	|
> | circular_adjacent_pair	| 单向环状相邻对	|

#### [二部](test/arrangement/bipartite.cpp)

> | 排布					| 含义		|
> | ---					| ---		|
> | cartesian_pair		| 笛卡尔积对	|
> | zip_pair_truncation	| 截断对齐对	|

#### [带填充二部](test/arrangement/bipartite_padding.cpp)

> | 排布					| 含义		|
> | ---					| ---		|
> | zip_pair_padding	| 填充对齐对	|

### 量词行为

#### [短路求值](test/behavior/quantifier/short_circuit_evaluation.cpp)

> 量词支持短路求值，即在结果确定后，不再对后续序列进行谓词求值。  
> 注意，结果确定并不意味着量词被满足。

#### [德摩根定律](test/behavior/quantifier/de_morgan_law.cpp)

> 德摩根第一定律：合取的否命题等价于否命题的析取。  
> ¬(P1 ∧ P2 ∧ ... ∧ Pn) ≡ (¬P1) ∨ (¬P2) ∨ ... ∨ (¬Pn)
> 
> 德摩根第二定律：析取的否命题等价于否命题的合取。  
> ¬(P1 ∨ P2 ∨ ... ∨ Pn) ≡ (¬P1) ∧ (¬P2) ∧ ... ∧ (¬Pn)
> 
> 用于验证 `all_of`、`any_of`、`none_of` 和 `not_every` 的行为正确性。

### 退化

#### [CV、引用、指针之矩阵](test/degeneracy/cv_ref_ptr_matrix.cpp)

> 在针对模板参数的验证场景中，当涉及 `const` 和 `volatile` 限定符、引用和指针声明符时，模板参数的身份同一性可能会发生非预期的偏离。  
> 本测试通过构建不同限定符、声明符组合的矩阵，验证 Logicwise 在这些边缘情况下的行为正确性。

#### [不可遍历规模](test/degeneracy/non_traversable_extent.cpp)

> 对于存在元容器的场景，当规模不可遍历时，由于不存在需要被谓词验证的单一变量或变量组，因此不能保证可以获取有效的元元素（类型或值）作为探针。  
> 此时，不对谓词形状（参数数量和参数性质）进行约束。  
> 即空集能匹配任何形状的谓词。

<!-------------------------------------------------------------------------------->

## 🎬 幕后碎碎念

### 注意事项

原则上 `namespace logicwise::detail` 中的内容应该仅供内部使用，而且并不保证稳定性。除非你知道自己在做什么，否则请不要直接使用或者依赖其中的内容。  
你可以使用其他 `namespace logicwise` 中的内容，前提是它们没有被标记为 `[未实现]`。

### 副作用

目前 Logicwise 的定位就是验证，千万别往回调函数里面塞副作用啊！并且不要把它当遍历框架用...

呃，我知道下面这段代码能编译：

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

但是这也太怪了吧！<strong>验证模式并不保证统一的遍历方向。</strong>答应我，别这么写好吗。  
~~（如果你们真的有需求，或许我会考虑添加遍历模式的...）~~  
（遍历模式已实装，但我还没来得及测试。）

### 已知编译器问题

关于编译器兼容性和已知问题的详细信息，详见[已知编译器问题](KNOWN_COMPILER_ISSUES.zh-Hans.md)。

### 未竟之事

虽然在包装器和关系那里说了一大堆，但我还没来得及测试它们。主要是因为它们属于更高一层级的体系，而且有的依赖功能还没实现。暂且就把它们当做附赠功能吧。

性能这块改天再说，目前还是聚焦于保障正确性吧。

标记为 `[未实现]` 的功能也别当真哈，都是写着玩的。

<!-------------------------------------------------------------------------------->

## 📜 许可证

```text
MIT License
Copyright © 2026 青蛙歌唱 (@frog-singing)
```

详见 [LICENSE](LICENSE) 文件。

<!-------------------------------------------------------------------------------->

## 💡 你好，逻辑！

```cpp
std::cout << "Hello Logic!" << std::endl;
```

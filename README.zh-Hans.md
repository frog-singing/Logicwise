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
				量词 × 组织 × 范围 × 谓词
			</strong>
		</td>
	</tr>
</table>

*声明式验证，让代码说人话。*

</div>

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

[【序章偷跑】想不想看看你能用这些量词做点什么？](test/mode/validation/quantifier.cpp)

### [8+ 种组织](library/logicwise/arrangement.h)

#### 逐元素

| 组织		| 含义		|
| ---		| ---		|
| element	| 单个元素	|

#### 逐对

| 组织						| 含义			|
| ---						| ---			|
| permutation_pair			| 全排列对		|
| combination_pair			| 保序组合对		|
| linear_adjacent_pair		| 单向线性相邻对	|
| circular_adjacent_pair	| 单向环状相邻对	|

#### 二部

| 组织					| 含义		|
| ---					| ---		|
| cartesian_pair		| 笛卡尔积对	|
| zip_pair_truncation	| 截断对齐对	|
| zip_pair_padding		| 填充对齐对	|

量词和组织都支持扩展！

<!-------------------------------------------------------------------------------->

## 📦 容器适配

| 容器			| 要求								| 举例		| 反例		|
| ---			| ---								| ---		| ---		|
| **类型列表**	| 所有参数都是类型的 <br> 实例化类模板		| 1. `wrapper::type_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<bool, int, double, char>` <br> 2. `std::tuple<bool, int, double, char>` | `std::array<int, 4>` <br> <br> _* 第二位参数并非类型_ |
| **值列表**		| 所有参数都是值（NTTP）的 <br> 实例化类模板 | `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;`<true, 2, 3.0, '4'>` | `std::make_index_sequence<4>` <br> <br> _* 该别名的实际类型为： <br> &nbsp;&nbsp;`std::integer_sequence` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<std::size_t, 0, 1, 2, 3>`， <br> &nbsp;&nbsp;第一位参数并非值_ |
| **类向量容器**	| 内存连续， <br> 均摊 O(1) 时间获取大小， <br> 支持下标访问 | 1. `std::vector<int>{ 1, 2, 3, 4 }` <br> 2. `std::array{ 1, 2, 3, 4 }` <br> 3. `int my_array[4]{ 1, 2, 3, 4 };` <br> 4. `std::span{ my_array }` | `std::list<int>{ 1, 2, 3, 4 }` <br> <br> _* 内存不连续_ |
| **类元组容器**	| 不直接支持类元组容器； <br> 提供了 `to_variant_array` <br> 方法来转换， <br> 并需要配合 `std::visit` 访问	| `container::to_variant_array(` <br> &nbsp;&nbsp;&nbsp;&nbsp;`std::make_tuple(true, 2, 3.0, '4')` <br> `)` | 1. `std::make_tuple(1, 2, 3, 4)` <br> 2. `wrapper::value_list` <br> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`<1, 2, 3, 4>{}` <br> <br> _* 不支持直接传入类元组容器_ |

*注：在二部组织的元容器和实例容器场景中，需要获取实例容器的静态大小，因此对实例容器的要求从类向量容器提升为*___类数组容器___*。*

<!-------------------------------------------------------------------------------->

## 🎭 逻辑小剧场

逻辑并不诞生自虚空，而是藏身于我们的日常体验和实际需求中。  
以下测试用例将以“剧目”的形式，带你走过那些不曾留意的角落，踏上一场从简单到抽象，最终回归日常的逻辑之旅。

> **坐好，演出马上开始！**

### 序章：你与量词的再次邂逅 🏃‍➡️💖🏃‍

#### [等等，啥是量词？](test/mode/validation/quantifier.cpp)

> 别慌！你一定已经在代码中使用过各种量词了，只是还不知道它们的大名而已。  
> 让我们重新认识一下它们，梳理一下基本用法。

### 第一章：我是其中之一&#8202;——逐元素 😒🤪😑

#### [借我，借我一双慧眼吧](test/mode/validation/elementwise.cpp)

> 以往，模板参数一旦进入模板，就好像掉入了黑洞，彻底销声匿迹。  
> 然而现在，你不仅能看见它们，还能审视它们！

### 第二章：关系构成整体——逐对

#### [占位符](test/mode/validation/pairwise.cpp)

> 样例文字

### 第三章：二部

#### [第 1 节：元](test/mode/validation/bipartite_meta.cpp)

> 样例文字

#### [第 2 节：实例](test/mode/validation/bipartite_instance.cpp)

> 样例文字

#### [第 3 节：元遇见实例](test/mode/validation/bipartite_meta_and_instance.cpp)

> 样例文字

<!-------------------------------------------------------------------------------->

## 🤔 设计哲学

说人话：通过语法直接对逻辑建模，只需要你一点点的数学思维

三界统一：类型、常量、实例

零成本抽象

<!-------------------------------------------------------------------------------->

## 🏭 逻辑工厂

wrapper

relation

<!-------------------------------------------------------------------------------->

## 🌀 逻辑边境

### 组织行为

验证 1D 到 2D 拓扑空间的索引生成纯度与对齐安全性

#### [逐元素](test/arrangement/elementwise.cpp)

> 样例文字

#### [逐对](test/arrangement/pairwise.cpp)

> 样例文字

#### [二部](test/arrangement/bipartite.cpp)

> 样例文字

#### [带填充二部](test/arrangement/bipartite_padding.cpp)

> 样例文字

### 量词行为

#### [短路求值](test/behavior/quantifier/short_circuit_evaluation.cpp)

> 样例文字

#### [德摩根定律](test/behavior/quantifier/de_morgan_law.cpp)

> 样例文字

### 退化

#### [CV、引用、指针之矩阵](test/degeneracy/cv_ref_ptr_matrix.cpp)

> 样例文字

#### [不可遍历规模](test/degeneracy/non_traversable_extent.cpp)

> 样例文字

<!-------------------------------------------------------------------------------->

## 🎬 幕后碎碎念

- **包装器**：还没来得及测试
- **关系**：还没来得及测试

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

但是这也太怪了吧！<strong>验证模式并不保证统一的遍历方向。</strong>答应我，别这么写好吗。（如果你们真的有需求，或许我会考虑添加遍历模式的...）

### 已知编译器问题

关于编译器兼容性和已知问题的详细信息，请参见[已知编译器问题](KNOWN_COMPILER_ISSUES.zh-Hans.md)。

<!-------------------------------------------------------------------------------->

## 🔌 集成方式

### 开发环境要求

- **C++ 标准**：C++20 或更高版本
- **编译器支持**：任何支持 C\++20 的现代 C++ 编译器（MSVC、GCC、Clang）
- **构建系统**：CMake 3.21+

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
  GIT_REPOSITORY https://github.com/frog-singing/Logicwise.git
  GIT_TAG main
)

FetchContent_MakeAvailable(logicwise)

target_link_libraries(your_target PRIVATE logicwise::logicwise)
```

### 手动集成

下载 [Logicwise](https://github.com/frog-singing/Logicwise) 库，将 `Logicwise/library` 目录包含在你项目的包含路径中。

Logicwise 同时依赖了 [Manipond](https://github.com/frog-singing/Manipond) 库，因此你还需要下载 Manipond 库，然后将 `Manipond/exosuit_library` 和 `Manipond/meta_library` 目录包含在你项目的包含路径中。

推荐把 Manipond 放到 `Logicwise/external/Manipond` ，这是 `Logicwise/CMakeLists.txt` 中的预设路径。

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

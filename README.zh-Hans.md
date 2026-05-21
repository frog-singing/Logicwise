<div align="center">

# Logicwise

*一个 C++ 概念工厂*

[English](README.md) | 简体中文

</div>

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
<td align="center">
<strong style="font-size: 1.5em;">
量词 × 组织 × 范围 × 谓词
</strong>
</td>
</tr>
</table>

声明式验证，让代码说人话。

</div>

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

## 📦 容器适配

| 容器		| 要求		| 举例		| 反例		|
| ---		| ---		| ---		| ---		|
| 类型列表	| 所有模板参数都是类型的类实例	| type_list<bool, int, double, char> <br> std::tuple<bool, int, double, char> |		|
| 值列表		| 所有模板参数都是值的类实例	| value_list<true, 2, 3.0, '4'> |		|
| 类向量容器	|	| std\::vector\<int>{1, 2, 3, 4} <br> int[4]{1, 2, 3, 4} <br> std::span\{ my_array } |		|
| 类元组容器	|	| container\::to_variant_array( <br> &nbsp;&nbsp;&nbsp;&nbsp;std::make_tuple(true, 2, 3.0, '4') <br> ) |		|

注：在二部的元容器和实体容器场景中，需要获取实体容器的静态大小，即对实体容器的要求从类向量容器升级为类数组容器

## 🎭 逻辑小剧场

我们不希望提供死板、无聊的 API 手册。  
以下测试用例将以“剧本”的形式，带你经历一场从逻辑公理到业务实战的旅程。  
坐好，演出马上开始！

### 序章：你与量词的再次邂逅 🏃‍➡️💖🏃‍

#### [等等，啥是量词？](test/mode/validation/quantifier.cpp)

别慌！你一定已经在代码中使用过各种量词了，只是还不知道它们的大名而已。  
让我们重新认识一下它们，梳理一下基本用法。

### [第一章：逐元素](test/mode/validation/elementwise.cpp)

### [第二章：逐对](test/mode/validation/pairwise.cpp)

### 第三章：二部

#### [第 1 节：元](test/mode/validation/bipartite_meta.cpp)

#### [第 2 节：实例](test/mode/validation/bipartite_instance.cpp)

#### [第 3 节：元遇见实例](test/mode/validation/bipartite_meta_and_instance.cpp)

## 🤔 设计哲学

逻辑结构直接体现在语法中。


## 🌀 逻辑边境

### 组织行为

#### [逐元素](test/arrangement/elementwise.cpp)

#### [逐对](test/arrangement/pairwise.cpp)

#### [二部](test/arrangement/bipartite.cpp)

#### [带填充二部](test/arrangement/bipartite_padding.cpp)

### 量词行为

#### [短路求值](test/behavior/quantifier/short_circuit_evaluation.cpp)

#### [德摩根定律](test/behavior/quantifier/de_morgan_law.cpp)

### 退化

#### [CV、引用、指针之矩阵](test/degeneracy/cv_ref_ptr_matrix.cpp)

#### [不可遍历规模](test/degeneracy/non_traversable_extent.cpp)

## 🎬 幕后碎碎念

## 🔌 集成方式

## 📜 许可证

## 💡 你好，逻辑！

```cpp
std::cout << "Hello Logic!" << std::endl;
```

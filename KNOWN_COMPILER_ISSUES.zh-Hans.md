# Logicwise 已知编译器问题

本文记录 Logicwise 测试用例在主流 C++20 编译器上遇到的已知编译问题。它们集中出现在模板元编程、受约束偏特化、类类型 NTTP、指针 NTTP 和 `nullptr` NTTP 等语言边界上。

这些问题不代表 Logicwise 的测试目标一定错误。多数情况下，测试代码本身是为了验证 C++20 语义边界，因此更容易暴露编译器前端差异。

本文在 AI 的帮助下编写，并不代表作者本人完全理解编译器的工作原理。

## 测试环境

Logicwise 要求 C++20。

本测试所用编译器：

| 编译器             | 版本          | 测试结果          |
| ---               | ---           | ---               |
| MSVC              | 19.51.36246   | [问题1](#issue_1) |
| Clang (clang-cl)  | 22.1.3        | [问题2](#issue_2) |
| Clang (clang++)   | 22.1.3        | [问题2](#issue_2) |
| GCC               | 16.1.0        | 通过所有测试       |

## 问题 1：MSVC 在 `cv_ref_ptr_matrix.cpp` 中对指针 NTTP 的值身份判断不同 <a id="issue_1"></a>

影响文件：

```text
test/degeneracy/cv_ref_ptr_matrix.cpp
```

现象：

文件中已有注释标出 MSVC 与 Clang/GCC 的差异：

```cpp
valuewise::same_as<V, ptr>; // same_as for Clang/GCC, !same_as for MSVC
```

涉及的值类似：

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

Logicwise/Manipond 的值身份判断使用类似写法：

```cpp
template<auto, auto>
constexpr bool is_same_value_v = false;

template<auto V>
constexpr bool is_same_value_v<V, V> = true;
```

Clang/GCC 认为这些表达式归约为同一个 `int*` 空指针值时满足 `valuewise::same_as<V, ptr>`；MSVC 可能给出 `false`。

判断：

高度疑似 MSVC 对指针 NTTP 的模板参数值等价处理与 Clang/GCC 不一致。它可能把 cv/ref 转换过程或表达式来源保留进了模板值身份判断，也可能只是把模板中不同索引的指针值视为不同身份的值。

临时处理：

- 按 `cv_ref_ptr_matrix.cpp` 中注释调整相关断言。
- 或在 MSVC 下将对应 `valuewise::same_as<V, ptr>` 改为更宽松的值等价判断，例如只检查：

```cpp
V == nullptr
```

或使用 `valuewise::equal_to`，视测试意图而定。

## 问题 2：Clang 在 `quantifier.cpp` 中出现顺序敏感的约束偏特化错误 <a id="issue_2"></a>

影响文件：

```text
test/mode/verification/quantifier.cpp
```

已观察环境：

```text
clang 22.1.6, x86_64-w64-windows-gnu
```

现象：

下面两个调用单独编译都可以通过：

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

空列表写在非空列表之前也可以通过；但非空列表先实例化，再编译空列表时，Clang 可能错误地进入只应匹配可遍历范围的偏特化，并最终报错：

```text
constraints not satisfied for alias template 'element'
Index = 18446744073709551615
```

原因分析：

Logicwise 中用于判断是否可遍历的约束类似：

```cpp
template<typename Arrangement, auto Extent>
concept TraversableExtent =
    std::same_as<typename Arrangement::extent_type, decltype(Extent)> &&
    !(typename Arrangement::light_index_traverser{ Extent }.done());
```

对于空 `type_list<>`，该约束应为 `false`，因此不应选择需要 `ProbeIndex` 的偏特化。实测中，独立断言：

```cpp
static_assert(!TraversableExtent<arrangement, extent_type{ as_type_list<type_list<>>::size }>);
```

可以通过。也就是说，约束本身能正确算出 `false`，但 Clang 在特定实例化顺序下仍选择了受该约束保护的偏特化。

判断：

高度疑似 Clang 对以下组合的前端处理问题：

```text
constrained member class template partial specialization
class-type non-type template parameter
dependent alias adapter
prior instantiation with another list size
```

临时处理：

- 使用 GCC 或 MSVC 编译该测试。
- 或在库侧给相关偏特化加显式非空保护，例如：

```cpp
requires TraversableExtent<Arrangement, extent_type{ as_type_list<TypeList>::size }>
      && (!as_type_list<TypeList>::empty)
```

这条保护在逻辑上是冗余的，但可以绕过 Clang 的错误路径。

## 推荐排查顺序

如果用户遇到编译失败，建议按以下顺序判断：

1. 确认启用了 C++20。
2. 确认失败文件是否为本文列出的三个测试文件之一。
3. 如果失败在 `cv_ref_ptr_matrix.cpp` 且使用 MSVC，按文件内注释调整对应断言。
4. 如果失败在 `quantifier.cpp` 且使用 Clang，优先怀疑 Clang 的约束偏特化问题。
5. 若业务项目只使用 Logicwise 头文件而不编译测试，可先关闭测试构建：

```bash
cmake -DLOGICWISE_BUILD_TESTING=OFF ...
```

## 当前建议

Logicwise 作为头文件库本身可被主流编译器使用。已知问题主要集中在测试套件中用于验证 C++20 元编程边界的极端用例。

若目标是集成库而不是验证编译器行为，建议关闭测试构建；若目标是参与库开发或验证语言边界，请保留这些测试，并根据编译器分别处理上述已知差异。

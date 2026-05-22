# Logicwise 已知编译器问题

本文记录 Logicwise 测试用例在主流 C++20 编译器上遇到的已知编译问题。它们集中出现在模板元编程、受约束偏特化、class-type NTTP 和指针/`nullptr` NTTP 等语言边界上。

这些问题不代表 Logicwise 的测试目标一定错误。多数情况下，测试代码本身是为了验证 C++20 语义边界，因此更容易暴露编译器前端差异。

## 环境要求

Logicwise 要求 C++20。

推荐优先尝试：

```text
GCC 15.x 或更新
Clang 22.x 或更新
MSVC 19.4x / Visual Studio 2022 或更新
```

不同编译器可能需要对个别测试文件做临时跳过或断言调整，见下文。

## 问题 1：Clang 在 `quantifier.cpp` 中出现顺序敏感的约束偏特化错误

影响文件：

```text
test/mode/validation/quantifier.cpp
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
      && (as_type_list<TypeList>::size > 0)
```

这条保护在逻辑上是冗余的，但可以绕过 Clang 的错误路径。

## 问题 2：GCC 在 `non_traversable_extent.cpp` 中拒绝 `nullptr` NTTP 的静态成员转发

影响文件：

```text
test/degeneracy/non_traversable_extent.cpp
```

已观察环境：

```text
GCC 15.2.0, MinGW-W64 x86_64-ucrt-posix-seh
```

现象：

GCC 能编译 `test/mode/validation/quantifier.cpp`，但在 `non_traversable_extent.cpp` 中遇到包含 `nullptr` 的 `value_list` 时失败，典型报错：

```text
error: 'nullptr' is not a constant expression
```

最小形态类似：

```cpp
template<auto Value>
struct value_element
{
    using value_type = decltype(Value);
    static constexpr value_type value{ Value };
};

template<auto Value>
struct value_list
{
    using element_type = typename value_element<Value>::value_type;
    static constexpr element_type element = value_element<Value>::value;
};

static_assert(value_list<nullptr>::element == nullptr);
```

Clang 接受上述代码，GCC 15.2 拒绝。若改为直接初始化：

```cpp
static constexpr element_type element = Value;
```

GCC 又可以接受。

判断：

高度疑似 GCC 对 `nullptr` 作为 NTTP 时，经由静态成员再次转发初始化的常量表达式处理存在问题。

临时处理：

- 使用 Clang 或 MSVC 编译该测试。
- 或临时移除/替换 `non_traversable_extent.cpp` 中 `unreachable_predicate_list` 里的 `nullptr` 元素。
- 或调整 `value_list` 的元素访问实现，避免通过中间静态成员转发 `nullptr`。

## 问题 3：MSVC 在 `cv_ref_ptr_matrix.cpp` 中对指针 NTTP 的值身份判断不同

影响文件：

```text
test/degeneracy/cv_ref_ptr_matrix.cpp
```

现象：

文件中已有注释标出 MSVC 与 Clang/GCC 的差异：

```cpp
valuewise::same_as<V, ptr>; // same_as for Clang, !same_as for MSVC
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

高度疑似 MSVC 对指针 NTTP 的模板参数值等价处理与 Clang/GCC 不一致。它可能把 cv/ref 转换过程或表达式来源保留进了模板值身份判断。

临时处理：

- 按 `cv_ref_ptr_matrix.cpp` 中注释调整相关断言。
- 或在 MSVC 下将对应 `valuewise::same_as<V, ptr>` 改为更宽松的值等价判断，例如只检查：

```cpp
V == nullptr
```

或使用 `valuewise::equal_to`，视测试意图而定。

## 推荐排查顺序

如果用户遇到编译失败，建议按以下顺序判断：

1. 确认启用了 C++20。
2. 确认失败文件是否为本文列出的三个测试文件之一。
3. 如果失败在 `quantifier.cpp` 且使用 Clang，优先怀疑 Clang 的约束偏特化问题。
4. 如果失败在 `non_traversable_extent.cpp` 且使用 GCC，优先怀疑 `nullptr` NTTP 转发问题。
5. 如果失败在 `cv_ref_ptr_matrix.cpp` 且使用 MSVC，按文件内注释调整对应断言。
6. 若业务项目只使用 Logicwise 头文件而不编译测试，可先关闭测试构建：

```bash
cmake -DLOGICWISE_BUILD_TESTING=OFF ...
```

## 当前建议

Logicwise 作为头文件库本身可被主流编译器使用。已知问题主要集中在测试套件中用于验证 C++20 元编程边界的极端用例。

若目标是集成库而不是验证编译器行为，建议关闭测试构建；若目标是参与库开发或验证语言边界，请保留这些测试，并根据编译器分别处理上述已知差异。


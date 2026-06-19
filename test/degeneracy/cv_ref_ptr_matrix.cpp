// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <concepts> // std::same_as
#include <iostream> // std::cout, std::endl


// If this file compiles successfully, then all logical assertions have passed.

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement;
	using namespace logicwise::wrapper;

	// type --------------------------------------------------------------------------------

	static_assert(
		rangewise<exactly<1>, element>
		::in<type_list<
			int,	const int,		volatile int,	const volatile int,
			int&,	int&&,			const int&,		const int&&,
			int*,	int* const,		const int*,		const int* const,
			int*&,	int*&&,			int**,			int* const* const
		>>()
		.satisfies([] <typename T> { return std::same_as<T, int>; }),
		"type uniqueness"
	);

	static_assert(
		rangewise<none_of, combination_pair>
		::in<type_list<
			int,	const int,		volatile int,	const volatile int,
			int&,	int&&,			const int&,		const int&&,
			int*,	int* const,		const int*,		const int* const,
			int*&,	int*&&,			int**,			int* const* const
		>>()
		.satisfies([] <typename T1, typename T2> { return std::same_as<T1, T2>; }),
		"type uniqueness"
	);

	// value --------------------------------------------------------------------------------

	constexpr int value{ 42 };
	constexpr int* ptr{ nullptr };
	constexpr int** ptr_ptr{ nullptr };

	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int)(value),	(const int)(value),	(volatile int)(value),	(const volatile int)(value),
			(int&)(value),	(int&&)(value),		(const int&)(value),	(const int&&)(value)
		>>()
		.satisfies([] <auto V> {
			return std::same_as<decltype(V), int> && V == 42 &&
				valuewise::same_as<V, value>;
		}),
		"value equivalence"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int)(value),	(const int)(value),	(volatile int)(value),	(const volatile int)(value),
			(int&)(value),	(int&&)(value),		(const int&)(value),	(const int&&)(value)
		>>()
		.satisfies([] <auto& V> {
			return std::same_as<decltype(V), const int&> && V == 42 &&
				valuewise::same_as<V, value>;
		}),
		"value equivalence"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int)(value),	(const int)(value),	(volatile int)(value),	(const volatile int)(value),
			(int&)(value),	(int&&)(value),		(const int&)(value),	(const int&&)(value)
		>>()
		.satisfies([] <auto&& V> {
			return std::same_as<decltype(V), const int&> && V == 42 &&
				valuewise::same_as<V, value>;
		}),
		"value equivalence"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int*)(ptr),	(int* const)(ptr),	(int* volatile)(ptr),	(int* const volatile)(ptr),
			(int*&)(ptr),	(int*&&)(ptr),		(int* const&)(ptr),		(int* const&&)(ptr)
		>>()
		.satisfies([] <auto V> {
			return std::same_as<decltype(V), int*> && V == nullptr &&
#if defined(_MSC_VER)
				!valuewise::same_as<V, ptr>; // !same_as for MSVC
#else
				valuewise::same_as<V, ptr>; // same_as for Clang/GCC
#endif
		}),
		"value equivalence"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int*)(ptr),	(int* const)(ptr),	(int* volatile)(ptr),	(int* const volatile)(ptr),
			(int*&)(ptr),	(int*&&)(ptr),		(int* const&)(ptr),		(int* const&&)(ptr)
		>>()
		.satisfies([] <auto& V> {
			return std::same_as<decltype(V), int* const&> && V == nullptr &&
#if defined(_MSC_VER)
				!valuewise::same_as<V, ptr>; // !same_as for MSVC
#else
				valuewise::same_as<V, ptr>; // same_as for Clang/GCC
#endif
		}),
		"value equivalence"
	);
	
	static_assert(
		rangewise<all_of, element>
		::in<value_list<
			(int*)(ptr),	(int* const)(ptr),	(int* volatile)(ptr),	(int* const volatile)(ptr),
			(int*&)(ptr),	(int*&&)(ptr),		(int* const&)(ptr),		(int* const&&)(ptr)
		>>()
		.satisfies([] <auto&& V> {
			return std::same_as<decltype(V), int* const&> && V == nullptr &&
#if defined(_MSC_VER)
				!valuewise::same_as<V, ptr>; // !same_as for MSVC
#else
				valuewise::same_as<V, ptr>; // same_as for Clang/GCC
#endif
		}),
		"value equivalence"
	);

	static_assert(
		rangewise<all_of, combination_pair>
		::in<value_list<
			(int*)(ptr), (const int*)(ptr), (volatile int*)(ptr), (const volatile int*)(ptr)
		>>()
		.satisfies([] <auto V1, auto V2> {
			return V1 == V2 && !std::same_as<decltype(V1), decltype(V2)> &&
				valuewise::equal_to<V1, V2> && !valuewise::same_as<V1, V2>;
		}),
		"value uniqueness"
	);

	static_assert(
		rangewise<all_of, combination_pair>
		::in<value_list<
			(int**)(ptr_ptr), (int* const*)(ptr_ptr), (int* volatile*)(ptr_ptr), (int* const volatile*)(ptr_ptr)
		>>()
		.satisfies([] <auto V1, auto V2> {
			return V1 == V2 && !std::same_as<decltype(V1), decltype(V2)> &&
				valuewise::equal_to<V1, V2> && !valuewise::same_as<V1, V2>;
		}),
		"value uniqueness"
	);

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

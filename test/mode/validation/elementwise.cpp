// Logicwise
// Copyright (c) 2026 Frog Singing (@frog-singing)
// SPDX-License-Identifier: MIT

#include <logicwise.h>

#include <array> // std::array
#include <vector> // std::vector
#include <span> // std::span
#include <ranges> // std::ranges

#include <concepts> // std::same_as, std::integral
#include <type_traits> // std::remove_cv_t, std::is_integral
#include <utility> // std::move
#include <iostream> // std::cout, std::boolalpha, std::endl


// If this file compiles successfully, then all logical assertions have passed.

namespace logicwise::test
{
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h
	using namespace logicwise::wrapper;

	// integral refers to integer type

	// type wrapper --------------------------------------------------------------------------------

	using my_integral_pool = type_list<int, long, unsigned int, char>;
	// try to add more integral types, such as bool, short, long long, etc.

	template<typename T>
	concept MyIntegral =
		rangewise<any_of, element>
		::in<my_integral_pool>()
		.satisfies([] <typename Integral> { return std::same_as<std::remove_cv_t<T>, Integral>; });

	template<typename T>
	struct is_my_integral_with_value
	{
		static constexpr bool value{ MyIntegral<T> };
	};

	template<typename T>
	struct is_my_integral_with_operator_bool
	{
		constexpr operator bool() const noexcept { return MyIntegral<T>; }
	};

	// value wrapper --------------------------------------------------------------------------------

	template<MyIntegral Integral>
	struct my_integer
	{
		Integral value;
		constexpr my_integer(Integral v) : value(v) {}
		constexpr operator Integral() const noexcept { return value; }
	};

	template<auto V>
	struct is_positive_with_value
	{
		static constexpr bool value{ V > 0 };
	};

	template<auto V>
	struct is_positive_with_operator_bool
	{
		constexpr operator bool() const noexcept { return V > 0; }
	};

}

int main()
{
	using namespace logicwise;
	using namespace logicwise::quantifier;
	using namespace logicwise::arrangement; // check this out in logicwise/arrangement.h
	using namespace logicwise::wrapper;

	using namespace logicwise::test;

	// type wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, const long, volatile unsigned int, const volatile char>>()
		.satisfies([] <typename T> { return std::integral<T>; }),
		"type wrapper: integral"
	);

	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, const long, volatile unsigned int, const volatile char>>()
		.satisfies([] <typename T> { return MyIntegral<T>; }),
		"type wrapper: integral"
	);

	static_assert(
		rangewise<none_of, element>
		::in<type_list<double, void, int&, long&&, unsigned int*, char*&>>()
		.satisfies([] <typename T> { return std::integral<T>; }),
		"type wrapper: non-integral"
	);

	static_assert(
		rangewise<none_of, element>
		::in<type_list<double, void, int&, long&&, unsigned int*, char*&>>()
		.satisfies([] <typename T> { return MyIntegral<T>; }),
		"type wrapper: non-integral"
	);


	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, const long, volatile unsigned int, const volatile char>>()
		.satisfies<std::is_integral>(),
		"type wrapper: std trait"
	);

	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, const long, volatile unsigned int, const volatile char>>()
		.satisfies<is_my_integral_with_value>(),
		"type wrapper: trait with value"
	);

	static_assert(
		rangewise<all_of, element>
		::in<type_list<int, const long, volatile unsigned int, const volatile char>>()
		.satisfies<is_my_integral_with_operator_bool>(),
		"type wrapper: trait with operator bool"
	);

	// value wrapper --------------------------------------------------------------------------------

	static_assert(
		rangewise<all_of, element>
		::in<value_list<3.14, true, my_integer<int>{ 10 }, my_integer<const long>{ 20 }>> ()
		.satisfies([] <auto V> { return V > 0; }),
		"value wrapper: positive"
	);

	static_assert(
		rangewise<none_of, element>
		::in<value_list<-3.14, false, my_integer<unsigned int>{ 0u }, my_integer<const long>{ -20 } >> ()
		.satisfies([] <auto V> { return V > 0; }),
		"value wrapper: non-positive"
	);


	static_assert(
		rangewise<all_of, element>
		::in<value_list<3.14, true, my_integer<int>{ 10 }, my_integer<const long>{ 20 }>> ()
		.satisfies<is_positive_with_value>(),
		"value wrapper: trait with value"
	);

	static_assert(
		rangewise<all_of, element>
		::in<value_list<3.14, true, my_integer<int>{ 10 }, my_integer<const long>{ 20 }>> ()
		.satisfies<is_positive_with_operator_bool>(),
		"value wrapper: trait with operator bool"
	);

	// compile time vector-like container --------------------------------------------------------------------------------

	static constexpr std::array<int, 4> compile_time_array{ 1, 2, 3, 4 };

	static_assert(
		rangewise<all_of, element>
		::in(compile_time_array)
		.satisfies([] (MyIntegral auto v) { return v > 0; }),
		"compile time array"
	);

	constexpr int compile_time_raw_array[4][1]{ {1}, {2}, {3}, {4} };

	static_assert(
		rangewise<all_of, element>
		::in(compile_time_raw_array)
		.satisfies([] (auto&& v) { return v[0] > 0; }),
		"compile time raw array"
	);

	// runtime vector-like container --------------------------------------------------------------------------------

	std::cout << std::boolalpha;

	std::array<int, 4> runtime_array{};

	std::vector<int> runtime_vector{ 1, 2, 3, 4 };
	std::ranges::copy(runtime_vector, runtime_array.begin());

	bool runtime_vector_positivity =
		rangewise<all_of, element>
		::in(std::move(runtime_vector))
		.satisfies([] (MyIntegral auto v) { return v > 0; });

	std::cout << "runtime vector positivity: " << runtime_vector_positivity << std::endl;

	std::span runtime_span{ runtime_array };

	bool runtime_span_positivity =
		rangewise<all_of, element>
		::in(runtime_span)
		.satisfies([] (MyIntegral auto v) { return v > 0; });

	std::cout << "runtime span positivity: " << runtime_span_positivity << std::endl;

	std::cout << std::endl << "Hello Logic!" << std::endl;
}

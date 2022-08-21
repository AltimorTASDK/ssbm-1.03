#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

using ssize_t = std::make_signed_t<size_t>;

template<typename T, size_t size>
using c_array_t = T[size];

template<typename T>
constexpr auto is_void = std::is_same_v<T, void>;

// Like std::tuple_size_v, but accepts reference types
template<typename T>
constexpr auto sizeof_tuple = std::tuple_size_v<std::remove_reference_t<T>>;

// Extract values from a tuple of std::integral_constants
template<size_t N, typename T>
constexpr auto tuple_constant = std::tuple_element_t<N, T>::value;

template<typename T>
concept TupleLike = requires(T t) {
	[]<size_t ...I>(std::index_sequence<I...>, T t) {
		(std::get<I>(t), ...);
	}(std::make_index_sequence<sizeof_tuple<T>>(), t);
};

// Requires tuple-likes of specific length
template<typename T, size_t N>
concept FixedTuple = sizeof_tuple<T> == N;

template<auto min, auto max, typename head, typename ...tail>
constexpr auto smallest_int_impl()
{
	using ulimits = std::numeric_limits<std::make_unsigned_t<head>>;
	using slimits = std::numeric_limits<std::make_signed_t<head>>;

	if constexpr (min >= ulimits::lowest() && max <= ulimits::max())
		return std::make_unsigned_t<head>{};
	else if constexpr (min >= slimits::lowest() && max <= slimits::max())
		return std::make_signed_t<head>{};
	else
		return smallest_int_impl<min, max, tail...>();
}

// Find the smallest int type capable of holding the given range
template<auto min, auto max>
using smallest_int_t = decltype(smallest_int_impl<min, max, int8_t, int16_t, int32_t, int64_t>());

constexpr auto sum_tuple(TupleLike auto &&tuple)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return (std::get<I>(tuple) + ...);
	}(std::make_index_sequence<sizeof_tuple<decltype(tuple)>>());
}

// std::make_tuple(0...N)
template<size_t N>
constexpr auto range()
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::make_tuple(I...);
	}(std::make_index_sequence<N>());
}

// std::make_tuple(start...end)
template<size_t start, size_t end>
constexpr auto range()
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::make_tuple((I + start)...);
	}(std::make_index_sequence<end - start>());
}

// std::make_tuple(std::integral_constant<size_t, 0...N>())
template<size_t N>
constexpr auto constant_range()
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::make_tuple(std::integral_constant<size_t, I>()...);
	}(std::make_index_sequence<N>());
}

// std::make_tuple(std::integral_constant<size_t, start...end>())
template<size_t start, size_t end>
constexpr auto constant_range()
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::make_tuple(std::integral_constant<size_t, I + start>()...);
	}(std::make_index_sequence<end - start>());
}

template<size_t N>
constexpr auto for_range(auto &&callable, auto &&...args)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return callable.template operator()<I...>(
			std::forward<decltype(args)>(args)...);
	}(std::make_index_sequence<N>());
}

template<size_t start, size_t end>
constexpr auto for_range(auto &&callable, auto &&...args)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return callable.template operator()<(I + start)...>(
			std::forward<decltype(args)>(args)...);
	}(std::make_index_sequence<end - start>());
}

// Cartesian product of two tuples
constexpr auto tuple_product(TupleLike auto &&a, TupleLike auto &&b)
{
	return for_range<sizeof_tuple<decltype(a)>>([&]<size_t ...I> {
		return std::tuple_cat([&]<size_t i> {
			return for_range<sizeof_tuple<decltype(b)>>([&]<size_t ...J> {
				return std::make_tuple(std::make_tuple(std::get<i>(a),
				                                       std::get<J>(b))...);
			});
		}.template operator()<I>()...);
	});
}

// Invoke callable with a template argument list containing the type list of the
// cartesian product tuple of constant_range<N>()...
template<size_t ...N>
constexpr auto for_range_product(auto &&callable, auto &&...args)
{
	return std::apply([&]<typename... tuples>(tuples &&...) {
		return callable.template operator()<tuples...>(
			std::forward<decltype(args)>(args)...);
	}, tuple_product(constant_range<N>()...));
}

// Make a tuple with a given value repeated N times.
template<size_t N>
constexpr auto fill_tuple(auto &&value)
{
	return [&]<size_t ...I>(std::index_sequence<I...>) {
		return std::make_tuple((I, value)...);
	}(std::make_index_sequence<N>());
}

// Like std::bind_front
constexpr auto bind_back(auto &&callable, auto &&...args)
{
	return [&](auto &&...head) {
		return callable(std::forward<decltype(head)>(head)...,
		                std::forward<decltype(args)>(args)...);
	};
}

template<ssize_t start, ssize_t end>
constexpr auto slice_tuple(TupleLike auto &&tuple)
{
	// Python style indices
	constexpr auto size = (ssize_t)sizeof_tuple<decltype(tuple)>;

	constexpr auto real_start = start >= 0
		? std::min(start, size - 1)
		: std::max(0z, size + start);

	constexpr auto real_end = end >= 0
		? std::min(end, size)
		: std::max(0z, size + end);

	return for_range<real_start, real_end>([&]<size_t ...I> {
		return std::forward_as_tuple(std::get<I>(tuple)...);
	});
}

template<size_t start, size_t end>
constexpr auto slice(auto &&...args)
{
	return slice_tuple<start, end>(std::forward_as_tuple(args...));
}

// Create a tuple of the Nth elements of the given tuples. If a tuple has no Nth
// element, it's omitted from the result.
template<size_t N>
constexpr auto zip_at_index(TupleLike auto &&...tuples)
{
	return std::tuple_cat([&] {
		if constexpr (N < sizeof_tuple<decltype(tuples)>)
			return std::forward_as_tuple(std::get<N>(tuples));
		else
			return std::make_tuple();
	}()...);
}

// Similar to Python's itertools.zip_longest, but without a fill value.
constexpr auto zip(TupleLike auto &&...tuples)
{
	if constexpr (sizeof...(tuples) > 1) {
		constexpr auto longest = std::max(sizeof_tuple<decltype(tuples)>...);

		return for_range<longest>([&]<size_t ...I> {
			return std::make_tuple(zip_at_index<I>(tuples...)...);
		});
	} else {
		// If only one tuple, wrap each element in a tuple
		constexpr auto size = sizeof_tuple<decltype(tuples)...>;

		return for_range<size>([&]<size_t ...I> {
			return std::make_tuple(std::forward_as_tuple(std::get<I>(tuples...))...);
		});
	}
}

// Given a tuple of tuples, unpack the values of all contained tuples into a single continuous tuple
constexpr auto tuple_chain(TupleLike auto &&tuple)
{
	return std::apply([](auto &&...tuples) {
		return std::tuple_cat(tuples...);
	}, tuple);
}

// Use std::apply with multiple argument tuples, returning a tuple of results.
// void results are omitted from the tuple.
constexpr auto apply_multi(auto &&callable, TupleLike auto &&...tuples)
{
	return std::tuple_cat([&] {
		if constexpr (!is_void<decltype(std::apply(callable, tuples))>) {
			return std::make_tuple(std::apply(callable, tuples));
		} else {
			std::apply(callable, tuples);
			return std::make_tuple();
		}
	}()...);
}

// Given tuple A of length 3, tuple B of length 2, and tuple C of length 4,
// zip_apply is equivalent to the following:
//
// callable(std::get<0>(A), std::get<0>(B), std::get<0>(C))
// callable(std::get<1>(A), std::get<1>(B), std::get<1>(C))
// callable(std::get<2>(A), std::get<2>(C))
// callable(std::get<3>(C))
//
// The results of callable are returned as a tuple. void results are omitted
// from the tuple. If no results are returned, this function returns void.
constexpr auto zip_apply(auto &&callable, TupleLike auto &&...tuples)
{
	return std::apply([&](auto &&...args) {
		return apply_multi(callable, std::forward<decltype(args)>(args)...);
	}, zip(tuples...));
}

namespace detail {

constexpr auto array_cat_impl(size_t offset, auto &result, auto &&head, auto &&...tail) {
	std::copy(head.begin(), head.end(), result.begin() + offset);
	if constexpr (sizeof...(tail) != 0)
		array_cat_impl(offset + head.size(), result, tail...);
};

} // namespace detail

template<typename T, size_t ...sizes>
constexpr auto array_cat(const std::array<T, sizes> &...arrays)
{
	std::array<T, (sizes + ...)> result;
	detail::array_cat_impl(0, result, arrays...);
	return result;
}

// Wrap string literals so they can be passed as template params
template<typename T, size_t ...N>
struct string_literal {
	static constexpr auto size = ((N - 1) + ...);

	constexpr string_literal(const c_array_t<T, N> &...strings)
	{
		if constexpr(sizeof...(N) == 1) {
			std::copy_n(strings..., size + 1, value);
		} else {
			std::copy_n(array_cat(for_range<N - 1>([&]<size_t ...I>(const auto &str) {
				if constexpr (sizeof...(I) != 0)
					return std::array { str[I]... };
				else
					return std::array<T, 0>();
			}, strings)..., std::array { T { 0 } }).begin(), size + 1, value);
		}
	}

	T value[size + 1];
};

// Wrap a list of arbitrarily sized std::arrays as an array of raw pointers
template<typename ...component_types>
class multi_array {
	using tuple_type = std::add_const_t<std::tuple<component_types...>>;
	using data_type = decltype(std::get<0>(std::declval<tuple_type>()).data());
	using array_type = std::array<data_type, sizeof_tuple<tuple_type>>;

	const std::array<size_t, sizeof...(component_types)> sizes;
	const tuple_type tuple;
	const array_type array;

public:
	constexpr multi_array(multi_array<component_types...> &&other) :
		sizes(std::move(other.sizes)),
		tuple(std::move(other.tuple)),
		array(for_range<sizeof...(component_types)>([&]<size_t ...I> {
			return std::array { std::get<I>(tuple).data()... };
		}))
	{
	}

	constexpr multi_array(component_types &&...components) :
		sizes { components.size()... },
		tuple(std::forward_as_tuple(std::move(components)...)),
		array(for_range<sizeof...(components)>([&]<size_t ...I> {
			return std::array { std::get<I>(tuple).data()... };
		}))
	{
	}

	constexpr size_t size() const
	{
		return sizeof...(component_types);
	}

	constexpr size_t size(size_t index) const
	{
		return sizes[index];
	}

	constexpr const data_type *data() const
	{
		return array.data();
	}

	constexpr data_type operator[](size_t index) const
	{
		return array[index];
	}
};
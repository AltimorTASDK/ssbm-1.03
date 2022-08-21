#pragma once

#include "util/meta.h"
#include "util/operators.h"
#include <cmath>
#include <gctypes.h>
#include <utility>
#include <tuple>
#include <type_traits>

template<typename Base>
class vec_impl;

template<typename T>
concept Vector = requires(T t) { []<typename U>(vec_impl<U>){}(t); };

template<typename Base>
class vec_impl : public Base {
	template<typename OtherBase>
	friend class vec_impl;

	using elem_tuple = decltype(std::declval<const Base>().elems());
	using elem_type  = std::tuple_element_t<0, elem_tuple>;

	static constexpr auto elem_count = sizeof_tuple<elem_tuple>;

public:
	using Base::elems;

	static constexpr vec_impl zero = vec_impl(fill_tuple<elem_count>(elem_type{0}));
	static constexpr vec_impl one  = vec_impl(fill_tuple<elem_count>(elem_type{1}));

	static constexpr elem_type dot(const vec_impl &a, const vec_impl &b)
	{
		return sum_tuple(a.foreach(operators::mul, b.elems()));
	}

	// Component-wise min of two vectors
	static constexpr vec_impl min(const vec_impl &a, const vec_impl &b)
	{
		return vec_impl(a.foreach(operators::min, b.elems()));
	}

	// Component-wise max of two vectors
	static constexpr vec_impl max(const vec_impl &a, const vec_impl &b)
	{
		return vec_impl(a.foreach(operators::max, b.elems()));
	}

	// Component-wise min and max of two vectors
	static constexpr auto min_max(const vec_impl &a, const vec_impl &b)
	{
		return std::make_tuple(min(a, b), max(a, b));
	}

	// Component-wise lerp of two vectors
	static constexpr vec_impl lerp(const vec_impl &a, const vec_impl &b, auto t)
	{
		return vec_impl(a.foreach(bind_back(std::lerp, t), b.elems()));
	}

	constexpr vec_impl()
	{
		foreach(bind_back(operators::eq, elem_type{}));
	}

	constexpr vec_impl(const vec_impl &other)
	{
		*this = other;
	}

	template<typename ...T, typename = std::enable_if_t<sizeof...(T) == elem_count>>
	constexpr vec_impl(T ...values)
	{
		elems() = std::make_tuple(values...);
	}

	explicit constexpr vec_impl(elem_tuple &&tuple)
	{
		elems() = tuple;
	}

	template<typename OtherBase>
	explicit constexpr vec_impl(const vec_impl<OtherBase> &other)
	{
		if constexpr (elem_count > other.elem_count) {
			constexpr auto pad = elem_count - other.elem_count;
			elems() = std::tuple_cat(other.elems(), fill_tuple<pad>(elem_type{}));
		} else if constexpr (elem_count < other.elem_count) {
			elems() = slice_tuple<0, elem_count>(other.elems());
		} else {
			elems() = other.elems();
		}
	}

	template<size_t N>
	constexpr auto &get() { return std::get<N>(elems()); }

	template<size_t N>
	constexpr auto get() const { return std::get<N>(elems()); }

	constexpr auto foreach(auto &&callable, FixedTuple<elem_count> auto &&...tuples)
	{
		return zip_apply(callable, elems(), tuples...);
	}

	template<typename ...Tuples>
	constexpr auto foreach(auto &&callable, FixedTuple<elem_count> auto &&...tuples) const
	{
		return zip_apply(callable, elems(), tuples...);
	}

	constexpr auto length_sqr() const
	{
		return dot(*this, *this);
	}

	constexpr auto length() const
	{
		return std::sqrt(length_sqr());
	}

	constexpr auto normalized() const
	{
		const auto len = length();
		if (len != 0)
			return *this * (decltype(len) { 1 } / len);
		else
			return *this;
	}

	// Create a new vector by applying a function to each component
	template<Vector T = vec_impl>
	constexpr T map(auto &&callable) const
	{
		return T(foreach(callable));
	}

	constexpr vec_impl &operator=(const vec_impl &other)
	{
		elems() = other.elems();
		return *this;
	}

	constexpr vec_impl &operator+=(const vec_impl &other)
	{
		foreach(operators::add_eq, other.elems());
		return *this;
	}

	constexpr vec_impl operator+(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::add, other.elems()));
	}

	constexpr vec_impl &operator-=(const vec_impl &other)
	{
		foreach(operators::sub_eq, other.elems());
		return *this;
	}

	constexpr vec_impl operator-(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::sub, other.elems()));
	}

	constexpr vec_impl &operator*=(const vec_impl &other)
	{
		foreach(operators::mul_eq, other.elems());
		return *this;
	}

	constexpr vec_impl &operator*=(auto value)
	{
		foreach(bind_back(operators::mul_eq, value));
		return *this;
	}

	constexpr vec_impl operator*(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::mul, other.elems()));
	}

	constexpr vec_impl operator*(auto value) const
	{
		return vec_impl(foreach(bind_back(operators::mul, value)));
	}

	constexpr vec_impl &operator/=(const vec_impl &other)
	{
		foreach(operators::div_eq, other.elems());
		return *this;
	}

	constexpr vec_impl &operator/=(auto value)
	{
		foreach(bind_back(operators::div_eq, value));
		return *this;
	}

	constexpr vec_impl operator/(const vec_impl &other) const
	{
		return vec_impl(foreach(operators::div, other.elems()));
	}

	constexpr vec_impl operator/(auto value) const
	{
		return vec_impl(foreach(bind_back(operators::div, value)));
	}

	constexpr bool operator==(const vec_impl &other) const
	{
		return elems() == other.elems();
	}

	constexpr vec_impl operator-() const
	{
		return vec_impl(foreach(operators::neg));
	}
};

template<typename T>
struct vec2_base {
	T x, y;
	constexpr auto elems()       { return std::tie(x, y); }
	constexpr auto elems() const { return std::make_tuple(x, y); }
};

using vec2 = vec_impl<vec2_base<f32>>;
using vec2c = vec_impl<vec2_base<s8>>;
using vec2b = vec_impl<vec2_base<u8>>;
using vec2i = vec_impl<vec2_base<s32>>;
using vec2d = vec_impl<vec2_base<f64>>;

template<typename T>
struct vec3_base {
	T x, y, z;
	constexpr auto elems() { return std::tie(x, y, z); }
	constexpr auto elems() const { return std::make_tuple(x, y, z); }

	static constexpr vec_impl<vec3_base> cross(const auto &a, const auto &b)
	{
		return vec_impl<vec3_base>(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}
};

using vec3  = vec_impl<vec3_base<f32>>;
using vec3i = vec_impl<vec3_base<s32>>;
using vec3d = vec_impl<vec3_base<f64>>;

template<typename T>
struct vec4_base {
	T x, y, z, w;
	constexpr auto elems()       { return std::tie(x, y, z, w); }
	constexpr auto elems() const { return std::make_tuple(x, y, z, w); }
};

using vec4  = vec_impl<vec4_base<f32>>;
using vec4i = vec_impl<vec4_base<s32>>;
using vec4d = vec_impl<vec4_base<f64>>;

template<typename T>
struct color_util {
	static constexpr auto max_value =
		std::is_floating_point_v<T> ? T{1} : std::numeric_limits<T>::max();
};

template<typename T>
struct color_rgb_base : color_util<T> {
	using color_util<T>::max_value;

	static constexpr vec_impl<color_rgb_base> white =
		{ max_value, max_value, max_value };

	T r, g, b;
	constexpr auto elems()       { return std::tie(r, g, b); }
	constexpr auto elems() const { return std::make_tuple(r, g, b); }
};

using color_rgb     = vec_impl<color_rgb_base<u8>>;
using color_rgb_f32 = vec_impl<color_rgb_base<f32>>;

template<typename T>
struct color_rgba_base : color_util<T> {
	using color_util<T>::max_value;

	static constexpr vec_impl<color_rgba_base> white =
		{ max_value, max_value, max_value, max_value };

	static constexpr auto hex(u32 value)
	{
		const auto r = (T)((value >> 24) & 0xFF) * (max_value / 255);
		const auto g = (T)((value >> 16) & 0xFF) * (max_value / 255);
		const auto b = (T)((value >>  8) & 0xFF) * (max_value / 255);
		const auto a = (T)((value >>  0) & 0xFF) * (max_value / 255);
		return vec_impl<color_rgba_base>(r, g, b, a);
	}

	T r, g, b, a;
	constexpr auto elems()       { return std::tie(r, g, b, a); }
	constexpr auto elems() const { return std::make_tuple(r, g, b, a); }
};

using color_rgba     = vec_impl<color_rgba_base<u8>>;
using color_rgba_f32 = vec_impl<color_rgba_base<f32>>;

struct uv_coord_base {
	f32 u, v;
	constexpr auto elems()       { return std::tie(u, v); }
	constexpr auto elems() const { return std::make_tuple(u, v); }
};

using uv_coord = vec_impl<uv_coord_base>;
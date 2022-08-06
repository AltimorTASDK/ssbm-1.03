#pragma once

#include "util/meta.h"
#include "operators.h"
#include <algorithm>
#include <tuple>
#include <utility>

template<typename T, size_t N, size_t M>
class matrix {
public:
	using array_type = float(*)[M];

	static constexpr auto row_count = N;
	static constexpr auto col_count = M;
	static constexpr auto elem_count = M * N;

	template<T factor>
	static constexpr matrix scale = std::make_from_tuple<matrix>(
		for_range_product<N, M>([]<typename... pairs> {
			return std::make_tuple([] {
				constexpr auto i = tuple_constant<0, pairs>;
				constexpr auto j = tuple_constant<1, pairs>;
				return i == j ? factor : 0;
			}()...);
		}));

	static constexpr matrix identity = scale<(T)1>;

	T elems[N * M];

	constexpr matrix() : elems({ 0 }) {}

	constexpr matrix(auto ...values) : elems(values...)
	{
		static_assert(sizeof...(values) == N * M);
	}

	constexpr T &operator[](size_t i, size_t j)
	{
		return elems[i * M + j];
	}

	constexpr T operator[](size_t i, size_t j) const
	{
		return elems[i * M + j];
	}

	template<size_t i>
	constexpr auto row()
	{
		return for_range<M>([&]<size_t ...J> {
			return std::tie(*this[i, J]...);
		});
	}

	template<size_t i>
	constexpr auto row() const
	{
		return for_range<M>([&]<size_t ...J> {
			return std::make_tuple(*this[i, J]...);
		});
	}

	constexpr auto rows()
	{
		return for_range<N>([&]<size_t ...I> {
			return std::make_tuple(row<I>()...);
		});
	}

	constexpr auto rows() const
	{
		return for_range<N>([&]<size_t ...I> {
			return std::make_tuple(row<I>()...);
		});
	}

	template<size_t j>
	constexpr auto col()
	{
		return for_range<N>([&]<size_t ...I> {
			return std::tie(*this[I, j]...);
		});
	}

	template<size_t j>
	constexpr auto col() const
	{
		return for_range<N>([&]<size_t ...I> {
			return std::make_tuple(*this[I, j]...);
		});
	}

	constexpr auto cols()
	{
		return for_range<M>([&]<size_t ...J> {
			return std::make_tuple(col<J>()...);
		});
	}

	constexpr auto cols() const
	{
		return for_range<M>([&]<size_t ...J> {
			return std::make_tuple(col<J>()...);
		});
	}

	constexpr auto as_tuple()
	{
		return for_range<N * M>([&]<size_t ...I> {
			return std::tie(elems[I]...);
		});
	}

	constexpr auto as_tuple() const
	{
		return for_range<N * M>([&]<size_t ...I> {
			return std::make_tuple(elems[I]...);
		});
	}

	template<size_t otherM>
	constexpr auto operator*(const matrix<T, M, otherM> &other) const
	{
		using result_type = matrix<T, N, otherM>;

		return std::make_from_tuple<result_type>(
			for_range_product<N, otherM>([&]<typename... pairs> {
				return std::make_tuple(([&] {
					constexpr auto i = tuple_constant<0, pairs>;
					constexpr auto j = tuple_constant<1, pairs>;
					return sum_tuple(zip_apply(
						operators::mul, row<i>(), other.template col<j>()));
				}())...);
			}));
	}

	constexpr auto operator*=(const auto &other)
	{
		*this = *this * other;
		return *this;
	}

	constexpr operator array_type() const
	{
		return (array_type)elems;
	}
};

using matrix3x3 = matrix<float, 3, 3>;
using matrix3x4 = matrix<float, 3, 4>;
using matrix4x4 = matrix<float, 4, 4>;

constexpr matrix4x4 ortho_projection(
	float t, float b,
	float l, float r,
	float n, float f)
{
	return matrix4x4 {
		2.f / (r - l), 0.f,            0.f,           -(r + l) / (r - l),
		0.f,           2.f / (t - b),  0.f,           -(t + b) / (t - b),
		0.f,           0.f,           -2.f / (f - n), -(f + n) / (f - n),
		0.f,           0.f,            0.f,            1.f
	};
}

#pragma once

#include "os/os.h"
#include "util/meta.h"
#include <array>
#include <cstddef>
#include <new>
#include <utility>

template<typename T, size_t N>
class objpool {
	using index_type = smallest_int_t<0, N - 1>;

	static constexpr auto NO_INDEX = N;

	static constexpr auto index_list = for_range<N>([]<size_t ...I> {
		return std::array { (index_type)I... };
	});

	union {
		T pool[N];
	};

	// Store free indices
	index_type free_index;
	std::array<index_type, N> free_list = index_list;

public:
	objpool()
	{
	}

	~objpool()
	{
		free_all();
	}

	T *alloc_uninitialized()
	{
		if (free_index >= N)
			PANIC("Failed to alloc from objpool");

		const auto index = free_list[free_index];

		if (index == NO_INDEX)
			PANIC("Failed to alloc from objpool");

		free_list[free_index++] = NO_INDEX;
		return &pool[index];
	}

	T *alloc(auto &&...params)
	{
		return new (alloc_uninitialized()) T(std::forward<decltype(params)>(params)...);
	}

	void free(T *object)
	{
		if (free_index == 0)
			PANIC("Failed to free to objpool");

		const auto index = object - pool;
		free_list[--free_index] = (index_type)index;
		object->~T();
	}

	void free_all()
	{
		auto indices = index_list;

		// Remove any indices that are in the free list
		for (size_t i = 0; i < N; i++) {
			const auto index = free_list[i];
			if (index != NO_INDEX)
				indices[index] = NO_INDEX;
		}

		for (size_t i = 0; i < N; i++) {
			const auto index = indices[i];
			if (index != NO_INDEX)
				free(&pool[index]);
		}
	}
};
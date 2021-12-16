#pragma once

#include "os/os.h"
#include "util/bitmask.h"
#include "util/math.h"
#include <array>
#include <cstddef>
#include <new>
#include <utility>

#include "os/os.h"

template<typename T, size_t N>
class objpool {
	static constexpr auto HINT_COUNT = 8;
	static constexpr auto HINT_MASK = HINT_COUNT - 1;
	static constexpr auto NO_HINT = -1;

	static_assert(is_pow2(HINT_COUNT));

	union {
		T pool[N];
	};

	// Store bits indicating whether a pool index is in use
	bitmask<N> usage;

	// Store recently freed indices
	std::array<ssize_t, HINT_COUNT> hints;
	size_t hint_index;

	// Reserve an index in the pool
	size_t acquire_free_index()
	{
		const auto hint = hints[hint_index];

		if (hint != NO_HINT) {
			hints[hint_index] = NO_HINT;
			hint_index = (hint_index - 1) & HINT_MASK;
			usage.set(hint, true);
			return hint;
		}

		for (size_t index = 0; index < N; index++) {
			if (!usage.get(index)) {
				usage.set(index, true);
				return index;
			}
		}

		PANIC("Failed to alloc from objpool");
	}

public:
	objpool()
	{
		hints.fill(NO_HINT);
	}

	~objpool()
	{
		free_all();
	}

	T *alloc_uninitialized()
	{
		const auto index = acquire_free_index();

		if (index + 1 < N && hints[hint_index] == NO_HINT && !usage.get(index + 1)) {
			// Use next index for next allocation if available
			hints[hint_index] = index + 1;
		}

		return &pool[index];
	}

	T *alloc(auto &&...params)
	{
		auto *object = alloc_uninitialized();
		return new (object) T(std::forward<decltype(params)>(params)...);
	}

	void free(T *object)
	{
		const auto index = object - pool;
		usage.set(index, false);
		hint_index = (hint_index + 1) & HINT_MASK;
		hints[hint_index] = index;
		object->~T();
	}

	void free_all()
	{
		for (size_t index = 0; index < N; index++) {
			if (usage.get(index))
				free(&pool[index]);
		}
	}
};
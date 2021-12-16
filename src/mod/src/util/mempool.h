#pragma once

#include "hsd/tobj.h"
#include "util/compression.h"
#include "util/objpool.h"
#include <new>

class mempool {
	struct allocation {
		allocation *prev;
		allocation *next;
		const void *ptr;
		void(*free)(const void *ptr);
	};

public:
	static mempool *pool_list_head;

	inline static objpool<allocation, 32> entry_pool;
	inline static objpool<texture_swap, 32> texture_swap_pool;

	mempool *prev = nullptr;
	mempool *next = nullptr;

	allocation *head = nullptr;
	int ref_count = 0;

	// Forcibly free all mem pools (on scene change)
	static void free_all()
	{
		while (pool_list_head != nullptr) {
			pool_list_head->ref_count = 0;
			pool_list_head->free();
		}
	}

	// Returns old ref count
	int inc_ref()
	{
		auto old_ref_count = ref_count++;
		if (old_ref_count == 0)
			init();

		return old_ref_count;
	}

	// Returns new ref count
	int dec_ref()
	{
		if (ref_count != 0 && --ref_count == 0)
			free();

		return ref_count;
	}

	// Free all entries
	void reset()
	{
		auto *entry = head;
		while (entry != nullptr) {
			entry->free(entry->ptr);

			auto *next = entry->next;
			entry_pool.free(entry);
			entry = next;
		}

		head = nullptr;
	}

	template<typename T>
	T *add(T *ptr)
	{
		head = new (entry_pool.alloc_uninitialized()) allocation {
			.prev = nullptr,
			.next = head,
			.ptr = ptr,
			.free = [](const void *ptr) { delete (T*)ptr; }
		};

		if (head->next != nullptr)
			head->next->prev = head;

		return ptr;
	}

	texture_swap *add_texture_swap(const u8 *data, HSD_ImageDesc *image = nullptr)
	{
		auto *ptr = texture_swap_pool.alloc(data, image);

		head = new (entry_pool.alloc_uninitialized()) allocation {
			.prev = nullptr,
			.next = head,
			.ptr = ptr,
			.free = [](const void *ptr) { texture_swap_pool.free((texture_swap*)ptr); }
		};

		if (head->next != nullptr)
			head->next->prev = head;

		return ptr;
	}

	void remove(const void *ptr)
	{
		auto *entry = head;

		while (entry != nullptr) {
			if (entry->ptr != ptr) {
				entry = entry->next;
				continue;
			}

			entry->free(entry->ptr);
			if (entry->prev != nullptr)
				entry->prev->next = entry->next;
			else
				head = entry->next;

			if (entry->next != nullptr)
				entry->next->prev = entry->prev;

			entry_pool.free(entry);
			return;
		}
	}

private:
	void init()
	{
		head = nullptr;

		prev = nullptr;
		next = pool_list_head;
		pool_list_head = this;

		if (next != nullptr)
			next->prev = this;
	}

	void free()
	{
		reset();

		// Unlink
		if (prev != nullptr)
			prev->next = next;
		else
			pool_list_head = next;

		if (next != nullptr)
			next->prev = prev;
	}
};
#pragma once

#include <list>

#include "melee/scene.h"

class mempool {
	struct allocation {
		allocation *prev;
		allocation *next;
		const void *ptr;
		void(*free)(const void *ptr);
	};

public:
	static mempool *pool_list_head;

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

	template<typename T>
	T *add(T *ptr)
	{
		head = new allocation {
			.prev = nullptr,
			.next = head,
			.ptr = ptr,
			.free = [](const void *ptr) { delete (T*)ptr; }
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

			delete entry;
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
		auto *entry = head;
		while (entry != nullptr) {
			entry->free(entry->ptr);

			auto *next = entry->next;
			delete entry;
			entry = next;
		}
		
		head = nullptr;

		// Unlink
		if (prev != nullptr)
			prev->next = next;
		else
			pool_list_head = next;
			
		if (next != nullptr)
			next->prev = prev;
	}
};
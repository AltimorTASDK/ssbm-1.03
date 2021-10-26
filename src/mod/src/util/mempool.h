#pragma once

class mempool {
	struct allocation {
		allocation *next;
		const void *ptr;
		void(*free)(const void *ptr);
	};

public:
	allocation *head = nullptr;
	int ref_count = 0;

	// Returns old ref count
	int inc_ref()
	{
		auto old_ref_count = ref_count++;
		if (old_ref_count == 0)
			init();
			
		return old_ref_count;
	}

	// Returns old ref count
	int dec_ref()
	{
		auto old_ref_count = ref_count--;
		if (ref_count == 0)
			free();
			
		return old_ref_count;
	}

	template<typename T>
	T *add(T *ptr)
	{
		head = new allocation {
			.next = head,
			.ptr = ptr,
			.free = [](const void *ptr) { delete (T*)ptr; }
		};

		return ptr;
	}

private:
	void init()
	{
		head = nullptr;
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
	}
};
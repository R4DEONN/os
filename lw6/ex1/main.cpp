#include <iostream>
#include <cassert>
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstddef>

class MemoryManager
{
public:
	MemoryManager(void* start, size_t size) noexcept
		: m_start(static_cast<char*>(start)), m_size(size), m_used(0)
	{
		assert((reinterpret_cast<std::uintptr_t>(start) % alignof(std::max_align_t)) == 0);
	}

	MemoryManager(const MemoryManager&) = delete;

	MemoryManager& operator=(const MemoryManager&) = delete;

	void* Allocate(size_t size, size_t align = sizeof(std::max_align_t)) noexcept
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		size_t aligned_size = (size + (align - 1)) & ~(align - 1);

		for (auto& block: m_free_blocks)
		{
			if (block.size >= aligned_size)
			{
				void* addr = block.addr;
				block.addr = static_cast<char*>(block.addr) + aligned_size;
				block.size -= aligned_size;

				if (block.size == 0)
				{
					m_free_blocks.erase(std::remove(m_free_blocks.begin(), m_free_blocks.end(), block),
						m_free_blocks.end());
				}

				return addr;
			}
		}

		if (m_used + aligned_size > m_size)
		{
			return nullptr;  // Нет достаточно памяти
		}

		void* addr = m_start + m_used;
		m_used += aligned_size;
		return addr;
	}

	void Free(void* addr) noexcept
	{
		if (addr == nullptr)
		{
			return; // Ничего не делаем, если указатель пустой
		}

		std::lock_guard<std::mutex> lock(m_mutex);

		m_free_blocks.push_back({ addr, 0 });  // Здесь мы будем просто добавлять адреса
	}

private:
	struct Block
	{
		Block(struct Block const &block)
		{
			addr = block.addr;
			size = block.size;
		}

		void* addr;  // Адрес начала блока
		size_t size; // Размер блока
	};

	char* m_start;        // Указатель на начало блока памяти
	size_t m_size;        // Размер блока памяти
	size_t m_used;        // Количество использованной памяти
	std::mutex m_mutex;   // Мьютекс для многопоточной безопасности
	std::vector<Block> m_free_blocks; // Список свободных блоков
};

int main()
{
	alignas(sizeof(std::max_align_t)) char buffer[1000];

	MemoryManager memoryManager(buffer, sizeof(buffer));

	auto ptr = memoryManager.Allocate(sizeof(double));

	// Конструируем значение в выделенном месте памяти
	auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
	assert(*value == 3.1415927);

	memoryManager.Free(ptr);
}
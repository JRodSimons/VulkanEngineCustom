#pragma once

class IEngine;

class Memory_System
{
public:
	bool MemorySystem_Initialize(IEngine* enginePtr);
	void MemorySystem_Shutdown(IEngine* enginePtr);

	ENGINE_API void* Mem_Allocate(size_t size);

	ENGINE_API void* Mem_Reallocate(void* memory, size_t new_size);

	ENGINE_API void Mem_Free(void* memory);

	ENGINE_API void* Mem_Copy(void* dest, const void* source, size_t size);


	void PrintAllocationInfo();

	int GetTotalAllocations() { return m_totalAllocations; }


private:
	unsigned int m_totalAllocations;
	size_t m_totalMemoryAllocated;
};

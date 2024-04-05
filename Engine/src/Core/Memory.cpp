#include "PCH_Engine.h"
#include "Memory.h"

#include "Core/Engine.h"
#include "Platform/Platform.h"

// TODO: Get rid of this ASAP!
static IEngine* g_enginePtr = 0;

bool Memory_System::MemorySystem_Initialize(IEngine* enginePtr)
{
	g_enginePtr = enginePtr;

	m_totalAllocations = 0;
	m_totalMemoryAllocated = 0;

	return true;
}

void Memory_System::MemorySystem_Shutdown(IEngine* enginePtr)
{
	LOG_VERBOSE("MemorySystem_Shutdown called");

	delete enginePtr->m_memorySystem;
	enginePtr->m_memorySystem = 0;

	return;
}

void* Memory_System::Mem_Allocate(size_t size)
{
	m_totalAllocations++;
	m_totalMemoryAllocated = m_totalMemoryAllocated += size;

	return g_enginePtr->m_PlatformLayer->Platform_Memory_Allocate(size);
}

void* Memory_System::Mem_Reallocate(void* memory, size_t new_size)
{
	return g_enginePtr->m_PlatformLayer->Platform_Memory_Reallocate(memory, new_size);
}

void Memory_System::Mem_Free(void* memory)
{
	m_totalAllocations--;
	m_totalMemoryAllocated = m_totalMemoryAllocated -= sizeof(memory);

	return g_enginePtr->m_PlatformLayer->Platform_Memory_Free(memory);
}

void* Memory_System::Mem_Copy(void* dest, const void* source, size_t size)
{
	return g_enginePtr->m_PlatformLayer->Platform_Memory_Copy(dest, source, size);
}

void Memory_System::PrintAllocationInfo()
{

}
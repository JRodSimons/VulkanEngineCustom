#pragma once
#include <Windows.h> // move asap!
#include <stdio.h>

#include <vulkan/vulkan.h>	// TODO: Vulkan platform layer?

typedef bool (*EXTFUNC)(void* data);

class IEngine;
struct ExtDLL;
struct ExtFunc;

struct ExtFunc
{
	const char* m_handle;
	EXTFUNC m_func;
};

struct ExtDLL
{
	const char* m_handle;
	void* m_data;
};

class IPlatform_Base
{
public:
	static bool Platform_Initialize(IEngine* enginePtr);
	bool Platform_Update();
	void Platform_Shutdown(IEngine* enginePtr);

	bool Platform_ProcessMessages();

	static ENGINE_API void Platform_GetWindowSize(unsigned int* out_width, unsigned int* out_height);
	ENGINE_API const char* Platform_GetVulkanExtensions();
	ENGINE_API VkWin32SurfaceCreateInfoKHR Platform_GetVulkanSurface(); // TODO: Vulkan platform layer?

	bool Platform_LoadDLL(const char* LibName, ExtDLL* out_container);
	bool Platform_UnloadDLL(ExtDLL* LibPtr, const char* LibHandle);
	bool Platform_LoadDLLFunction(ExtDLL* LibHandle, const char* FuncName, EXTFUNC* out_func);
	bool Platform_CallDLLFunction(EXTFUNC funcHandle, const char* funcName, void* funcData);

	void Platform_LogMessage(int level, const char* message);

	static ENGINE_API double Platform_GetAbsoluteTime();
	void Platform_Sleep(int ms);

	unsigned int Platform_GetProcessorCoreCount() { return m_numberOfProcessors; }

	// ============================================================================
	// PLATFORM MEMORY FUNCTIONS
	// ============================================================================
	void* Platform_Memory_Allocate(size_t size);
	void* Platform_Memory_Reallocate(void* memory, size_t new_size);
	void Platform_Memory_Free(void* memory);
	void* Platform_Memory_Copy(void* dest, const void* source, size_t num);

	// Clock
	inline static double m_privFreq;
	inline static LARGE_INTEGER m_startTime;

	// System info
	unsigned int m_numberOfProcessors;

	// TODO: Make configurable (like an array)
	ExtDLL m_RendererLibrary;
	EXTFUNC m_rendererLibraryInitialize;

	// std::vector<ExtFunc> m_libFuncs;
	EXTFUNC m_libFunc2;

	// TODO: Move this asap! Needed to stand up renderer... :(
	HWND hWnd;
	HINSTANCE h_Instance;
};

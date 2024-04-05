#ifdef PLAT_WINDOWS
#pragma once
#include "Platform.h"

#include <Windows.h>

class IEngine;

class IPlatform_Win32 : public IPlatform_Base
{
public:
	bool PlatformWin32_Initialize(IEngine* enginePtr);
	bool PlatformWin32_Update();
	void PlatformWin32_Shutdown(IEngine* enginePtr);

	void Platform_GetWindowSize(int* out_width, int* out_height) override;
	bool CreateWin32Window();

	IPlatform_Win32* Platform_GetWin32Layer() { return this; }

	// ============================================================================
	// PLATFORM LIBRARY FUNCTIONS
	// ============================================================================
	bool Platform_LoadDLL(const char* LibName) override;
	bool Platform_UnloadDLL(HMODULE* LibName) override;
	bool Platform_LoadDLLFunction(HMODULE* LibName, const char* FuncName) override;
	void Platform_CallDLLFunction(EXTFUNC* DLLFunc) override;

	// ============================================================================
	// PLATFORM MEMORY FUNCTIONS
	// ============================================================================
	void* Platform_Memory_Allocate(size_t size) override;
	void* Platform_Memory_Reallocate(void* memory, size_t new_size) override;
	void Platform_Memory_Free(void* memory) override;
	void* Platform_Memory_Copy(void* dest, const void* source, size_t num) override;


	HWND hWnd;
	HINSTANCE h_Instance;

	HMODULE m_extLibHandle;	// Make this more configurable (perhaps an array of DLLs?)

	EXTFUNC m_extFunc = 0;
};
#endif
#include "PCH_Engine.h"
#ifdef PLAT_WINDOWS

#include "Platform.h"
#include "Core/Engine.h"
#include "Core/Logging.h"
#include "Core/Event.h"
#include "Core/Input.h"
#include "Core/Memory.h"
#include "Core/String.h"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vulkan/vulkan.h>

static bool CreateWin32Window();
static int processWin32Input_Key(WPARAM wParam);
static int processWin32Input_MouseButton(UINT uMsg);
static int processWin32Input_MouseMove(LPARAM lParam);
static bool OnSize(void* data);
void enableColors();	// TEMP

static HWND hWnd;
static HINSTANCE h_Instance;
static LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
const char* engine_window_class{ "Engine_Window_Class" };

static IPlatform_Base* platformInterface = 0;
static IEngine* plat_enginePtr = 0;

const char* vulkanWin32Extension{ "VK_KHR_win32_surface" };

bool IPlatform_Base::Platform_Initialize(IEngine* enginePtr)
{
	//TODO: EXTREME TEMP, fix logging color stuff
	enableColors();

	plat_enginePtr = enginePtr;

	platformInterface = (IPlatform_Base*)plat_enginePtr->m_memorySystem->Mem_Allocate(sizeof(IPlatform_Base));

	// Register window resize event.
	enginePtr->m_eventSystem->EventRegister(EVENT_TYPE_WINDOW_RESIZE, &OnSize);
	
	if (!CreateWin32Window())
	{
		LOG_FATAL("CreateWin32Window failed! - Aborting application!");
		return false;
	}

	enginePtr->m_PlatformLayer = platformInterface;

	// Initialize clock frequency
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_privFreq = 1.0 / frequency.QuadPart;
	QueryPerformanceCounter(&m_startTime);

	SYSTEM_INFO sysInfo{};
	GetSystemInfo(&sysInfo);
	platformInterface->m_numberOfProcessors = sysInfo.dwNumberOfProcessors;

	LOG_INFO("Platform layer initialized");

	return true;
}

bool CreateWin32Window()
{
	// Create the window class.
	LPCSTR window_class_name = engine_window_class;

	platformInterface->h_Instance = GetModuleHandle(NULL);

	WNDCLASSA wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = platformInterface->h_Instance;
	wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = window_class_name;

	// Register the window class.
	RegisterClassA(&wc);

	HWND hWndInit = CreateWindowExA(
		0,
		window_class_name,
		"Engine Testbed",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		plat_enginePtr->m_width,
		plat_enginePtr->m_height,
		NULL,
		NULL,
		platformInterface->h_Instance,
		NULL);

	if (!hWndInit)
	{
		return false;
	}

	platformInterface->hWnd = hWndInit;

	// Show the window
	ShowWindow(platformInterface->hWnd, 1);

	return true;
}

bool IPlatform_Base::Platform_Update()
{
	return true;
}

void IPlatform_Base::Platform_Shutdown(IEngine* enginePtr)
{
	LOG_VERBOSE("Platform_Shutdown called");

	if (enginePtr && hWnd)
	{
		plat_enginePtr->m_eventSystem->EventUnregister(EVENT_TYPE_WINDOW_RESIZE, OnSize);

		DestroyWindow(hWnd);

		enginePtr->m_memorySystem->Mem_Free(platformInterface);

		enginePtr->m_PlatformLayer = 0;
		platformInterface = 0;
	}
}

bool IPlatform_Base::Platform_ProcessMessages()
{
	MSG msg = {};
	while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return true;
}

LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (GetKeyState(VK_ESCAPE) & 0x8000)
		{
			// Escape pressed... exit application // TODO!!! MOVE ELSEWHERE
			plat_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_APPLICATION_QUIT, 0);
			break;
		}
		else
		{
			// Prepare message data for input processing
			int processedKeyCode = processWin32Input_Key(wParam);

			// Send to the input system.
			plat_enginePtr->m_inputSystem->ProcessKeyInput(processedKeyCode, true);
			break;
		}
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// Prepare message data for input processing
			int processedKeyCode = processWin32Input_Key(wParam);

			// Send to the input system.
			plat_enginePtr->m_inputSystem->ProcessKeyInput(processedKeyCode, false);
			break;
		}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		int processedMouseCode = processWin32Input_MouseButton(uMsg);

		plat_enginePtr->m_inputSystem->ProcessMouseInput(NULL, NULL, processedMouseCode, false);
		break;
	}
		
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
			int processedMouseCode = processWin32Input_MouseButton(uMsg);

			plat_enginePtr->m_inputSystem->ProcessMouseInput(NULL, NULL, processedMouseCode, true);
			break;
	}
	case WM_MOUSEMOVE:
		processWin32Input_MouseMove(lParam);
		break;
		
	case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);

			void* eventData[] = { (void*)width, (void*)height };

			plat_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_WINDOW_RESIZE, eventData);
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ============================================================================
//  WIN32 Input Processing
// ============================================================================
int processWin32Input_Key(WPARAM wParam)
{
	switch (wParam)
	{
	case 0x41:
		return KEY_A;
		break;
	case 0x42:
		return KEY_B;
		break;
	case 0x43:
		return KEY_C;
		break;
	case 0x44:
		return KEY_D;
		break;
	case 0x45:
		return KEY_E;
		break;
	case 0x46:
		return KEY_F;
		break;
	case 0x47:
		return KEY_G;
		break;
	case 0x48:
		return KEY_H;
		break;
	case 0x49:
		return KEY_I;
		break;
	case 0x4A:
		return KEY_J;
		break;
	case 0x4B:
		return KEY_K;
		break;
	case 0x4C:
		return KEY_L;
		break;
	case 0x4D:
		return KEY_M;
		break;
	case 0x4E:
		return KEY_N;
		break;
	case 0x4F:
		return KEY_O;
		break;
	case 0x50:
		return KEY_P;
		break;
	case 0x51:
		return KEY_Q;
		break;
	case 0x52:
		return KEY_R;
		break;
	case 0x53:
		return KEY_S;
		break;
	case 0x54:
		return KEY_T;
		break;
	case 0x55:
		return KEY_U;
		break;
	case 0x56:
		return KEY_V;
		break;
	case 0x57:
		return KEY_W;
		break;
	case 0x58:
		return KEY_X;
		break;
	case 0x59:
		return KEY_Y;
		break;
	case 0x5A:
		return KEY_Z;
		break;
	default:
		return KEY_UNDEFINED;
		break;
	}
}

int processWin32Input_MouseButton(UINT uMsg)
{
	switch (uMsg)
	{
	case 513:
		return MOUSE_BUTTON_LEFTDOWN;
		break;
	case 514:
		return MOUSE_BUTTON_LEFTUP;
		break;
	case 516:
		return MOUSE_BUTTON_RIGHTDOWN;
		break;
	case 517:
		return MOUSE_BUTTON_RIGHTUP;
		break;
	case 519:
		return MOUSE_BUTTON_MIDDLEDOWN;
		break;
	case 520:
		return MOUSE_BUTTON_MIDDLEUP;
		break;
	default:
		return MOUSE_BUTTON_UNDEFINED;
		break;
	}
}

int processWin32Input_MouseMove(LPARAM lParam)
{
	float mouseXPos = 0;
	float mouseYPos = 0;

	if (lParam)
	{
		mouseXPos = LOWORD(lParam);
		mouseYPos = HIWORD(lParam);
	}

	plat_enginePtr->m_inputSystem->ProcessMouseInput(mouseXPos, mouseYPos, NULL, NULL);

	return true;
}

// ============================================================================
//  WIN32 Event Callbacks
// ============================================================================
bool OnSize(void* data)
{
	unsigned int width, height;
	platformInterface->Platform_GetWindowSize(&width, &height);

	// Minimize check...
	int isMinimized = IsIconic(platformInterface->hWnd);

	if (isMinimized == 0)
	{
		plat_enginePtr->m_isSuspended = false;
	}
	else
	{
		plat_enginePtr->m_isSuspended = true;
	}

	LOG_INFO("Engine Resized! w/h = %d, %d", width, height);
	return true;
}

bool PFN_Application_Quit(void* data)
{
	LOG_VERBOSE("Application_Quit() called");
	plat_enginePtr->m_bApplicationQuitCalled = true;
	DestroyWindow(platformInterface->hWnd);
	return true;
}

void IPlatform_Base::Platform_GetWindowSize(unsigned int* out_width, unsigned int* out_height)
{
	RECT rect;
	if (GetWindowRect(platformInterface->hWnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		*out_width = width;
		*out_height = height;
	}
}

const char* IPlatform_Base::Platform_GetVulkanExtensions()
{
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

VkWin32SurfaceCreateInfoKHR IPlatform_Base::Platform_GetVulkanSurface()
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = platformInterface->hWnd;
	createInfo.hinstance = platformInterface->h_Instance;

	return createInfo;
}

double IPlatform_Base::Platform_GetAbsoluteTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	double elapsedTime = (double)currentTime.QuadPart * m_privFreq;

	return elapsedTime;
}

void IPlatform_Base::Platform_Sleep(int ms)
{
	Sleep(ms);
}

// ============================================================================
//  WIN32 Memory Functions
// ============================================================================
void* IPlatform_Base::Platform_Memory_Allocate(size_t size)
{
	return malloc(size);
}

void* IPlatform_Base::Platform_Memory_Reallocate(void* memory, size_t new_size)
{
	return realloc(memory, new_size);
}

void IPlatform_Base::Platform_Memory_Free(void* memory)
{
	free(memory);
	memory = 0;
}

void* IPlatform_Base::Platform_Memory_Copy(void* dest, const void* source, size_t num)
{
	void* block = memcpy(dest, source, num);

	return block;
}

void enableColors()
{
	DWORD consoleMode;
	HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetConsoleMode(outputHandle, &consoleMode))
	{
		SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	}
}

bool IPlatform_Base::Platform_LoadDLL(const char* LibName, ExtDLL* out_container)
{
	bool fLib_RunTimeLinked = false;

	const char* Lib = string_format("%s%s", LibName, ".dll");

	HMODULE hInstLib;
	hInstLib = LoadLibrary(Lib);

	if (hInstLib != NULL)
	{
		out_container->m_handle = LibName;
		out_container->m_data = hInstLib;

		LOG_VERBOSE("'%s.dll' successfully loaded.", LibName);
		return true;
	}

	if (!fLib_RunTimeLinked)
	{
		LOG_ERROR("Failed to load external library: %s", Lib);
		return false;
	}
}

bool IPlatform_Base::Platform_UnloadDLL(ExtDLL* LibPtr, const char* LibHandle)
{
	bool fLib_FreeResult = false;

	const char* Lib = string_format("%s%s", LibHandle, ".dll");

	fLib_FreeResult = FreeLibrary((HMODULE)LibPtr->m_data);

	if (fLib_FreeResult)
	{
		LOG_INFO("Successfully unloaded external library");
		return true;
	}
	else
	{
		LOG_ERROR("Failed to unload external library");
		return false;
	}
}


bool IPlatform_Base::Platform_LoadDLLFunction(ExtDLL* LibHandle, const char* FuncName, EXTFUNC* out_func)
{

	EXTFUNC func_addr = (EXTFUNC)GetProcAddress((HMODULE)LibHandle->m_data, FuncName);
	ExtFunc func_handle{ FuncName, func_addr };

	*out_func = func_addr;

	if (func_addr != NULL)
	{
		LOG_INFO("Platform_LoadDLLFunction was successful!");
		return true;
	}
	else
	{
		LOG_ERROR("Platform_LoadDLLFunction was unsuccessful!");
		return false;
	}
}

bool IPlatform_Base::Platform_CallDLLFunction(EXTFUNC funcHandle, const char* funcName, void* funcData)
{
	funcHandle(&funcData);

	//LOG_ERROR("No external function named '%s' could be found!", funcName);
	return true;
}

void IPlatform_Base::Platform_LogMessage(int level, const char* message)
{
	// TODO: Cache this on startup instead of retrieving it every call?
	HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	static char levelColor[6] = { 2, 3, 1, 6, 4, 64 };	// Borrowed from Kohi
	SetConsoleTextAttribute(outputHandle, levelColor[level]);
	unsigned int length = strlen(message);
	WriteConsoleA(outputHandle, message, length, 0, 0);
}

#endif
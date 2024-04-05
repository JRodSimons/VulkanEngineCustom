#pragma once

typedef int b32; // 32-bit boolean (needed for Vulkan/other misc. cases)

// Exports
#if defined ENGINE_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API  __declspec(dllimport)
#endif
#if defined VULKAN_DLL
#define VULKAN_API  __declspec(dllexport)
#else
#define VULKAN_API  __declspec(dllimport)
#endif


// Platform detection

// Windows
#if defined _WIN32
#define PLAT_WINDOWS 1
#else
#define PLAT_WINDOWS 0
#endif

// Memory Macros

#define KIBIBYTES(x) (x * 1024)
#define MEBIBYTES(x) (x * (1024 * 1024))
#define GIBIBYTES(x) (x * (1024 * 1024 * 1024))
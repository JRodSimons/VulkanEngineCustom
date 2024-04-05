CustomsVulkan (To be renamed) - is the base of a Vulkan Rendering Engine written in C++.

A Visual Studio solution file can be built using the GenerateProjects script included in the root directory.
This solution will be built targetting VS 2019, but can be retargetted in the script file.

The solution includes four modules -

1. Engine - This is the core engine, built into a DLL. Contains the main loop and the frontend for most of the core subsystems.
2. Testapp - The primary executable, contains the entry point for the application.
3. Unit Tests - Builds into an executable, currently unused, but would be a separate process for performing unit tests.
4. VulkanRenderer - The core of the Vulkan Specific code.

The Vulkan SDK (specifically the C-API) is required to build and run this project!

This repo also contains 3rd-party code:
Premake - used as the build solution
tinyobjloader - currently unused - but is used to load .obj files on a separate branch
stb_image - used to load texture files.
glm - to handle 3d math

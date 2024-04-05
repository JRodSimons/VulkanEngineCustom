#include "Vulkan_Backend.h"

#include <Core/Engine.h>
#include <Core/Logging.h>
#include <Core/Event.h>
#include <Core/String.h>

#include <Platform/Platform.h>

#include <Renderer/Renderer_Interface.h>

// TODO: TEMP
#include <Math/Math_Types.h> 
#include <array>
// TODO: END TEMP

extern std::vector<Vertex_t> g_vertices =
{
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

extern std::vector<uint16_t> g_indices =
{
	0, 1, 2, 2, 3, 0
};

uint32_t GetIndexSize()
{
	return g_indices.size();
}

static std::vector<char> readFile(const std::string& filename);

struct RenderPacket_t;

static IEngine* g_enginePtr = 0;
static bool		g_framebufferResized = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallbackFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

static bool WindowResizeCallback(void* data);

// Set for double buffering (2) or triple buffering (3)
const int MAX_FRAMES_IN_FLIGHT = 2;

bool VulkanRendererInitialize(IEngine* enginePtr)
{
	enginePtr->m_RendererFrontend->m_RendererBackend = new IVulkanRenderer;
	enginePtr->m_RendererFrontend->m_RendererBackend->RendererBackend_Initialize(enginePtr);

	return true;
}

IVulkanRenderer::IVulkanRenderer(void)
{

}

bool IVulkanRenderer::RendererBackend_Initialize(IEngine* enginePtr)
{
	g_enginePtr = enginePtr;
	m_enginePtr = enginePtr;

	CreateVulkanInstance();
	CreateDebugMessenger();
	CreateSurface();
	SetupDevice();
	CreateSwapchain();
	CreateRenderpass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateSyncObjects();

	g_enginePtr->m_eventSystem->EventRegister(EVENT_TYPE_WINDOW_RESIZE, WindowResizeCallback);

	return true;
}

void IVulkanRenderer::RendererBackend_Shutdown()
{
	vkDeviceWaitIdle(m_device.GetLogicalDevice());
	// Shutdown renderer components in reverse order.
	// TODO: Custom allocator.

	m_swapchain.CleanupSwapchain(this);

	vkDestroyBuffer(m_device.GetLogicalDevice(), m_indexBuffer.GetBufferHandle(), nullptr);
	vkFreeMemory(m_device.GetLogicalDevice(), m_indexBuffer.GetBufferMemory(), nullptr);

	vkDestroyBuffer(m_device.GetLogicalDevice(), m_vertexBuffer.GetBufferHandle(), nullptr);
	vkFreeMemory(m_device.GetLogicalDevice(), m_vertexBuffer.GetBufferMemory(), nullptr);

	vkDestroyPipeline(m_device.GetLogicalDevice(), m_pipeline.GetPipelineHandle(), nullptr);
	vkDestroyPipelineLayout(m_device.GetLogicalDevice(), m_pipeline.GetPipelineLayout(), nullptr);
	vkDestroyRenderPass(m_device.GetLogicalDevice(), m_renderpass.GetRenderpassHandle(), nullptr);

	for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_device.GetLogicalDevice(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_device.GetLogicalDevice(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_device.GetLogicalDevice(), m_inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_device.GetLogicalDevice(), m_commandPool, nullptr);

	vkDestroyDevice(m_device.GetLogicalDevice(), nullptr);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	DestroyDebugger();
	vkDestroyInstance(m_instance, nullptr);

	g_enginePtr->m_eventSystem->EventUnregister(EVENT_TYPE_WINDOW_RESIZE, WindowResizeCallback);

	delete g_enginePtr->m_RendererFrontend->m_RendererBackend;

	g_enginePtr = 0;
	return;
}

void IVulkanRenderer::RendererBackend_Update(RenderPacket_t renderPacket)
{
	RendererBackend_DrawFrame();
}

void IVulkanRenderer::RendererBackend_DrawFrame()
{
	vkWaitForFences(m_device.GetLogicalDevice(), 1, &m_inFlightFences[m_currentFrameIndex], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_device.GetLogicalDevice(), m_swapchain.GetSwapchainHandle(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrameIndex], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		m_swapchain.RecreateSwapchain(this);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		AssertMsg("Failed to acquire swapchain image!");
	}

	vkResetFences(m_device.GetLogicalDevice(), 1, &m_inFlightFences[m_currentFrameIndex]);

	vkResetCommandBuffer(m_commandBuffers[m_currentFrameIndex], 0);
	VulkanCommandBuffer_t::RecordCommandBuffer(this, m_commandBuffers[m_currentFrameIndex], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrameIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrameIndex];

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrameIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrameIndex]) != VK_SUCCESS)
	{
		AssertMsg("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_swapchain.GetSwapchainHandle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_device.GetPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || g_framebufferResized) 
	{
		// HACK: Does this have to be here? Recreating the swapchain (obviously) crashes the program on shutdown...
		if (g_enginePtr->m_bApplicationQuitCalled == true)
		{
			return;
		}

		g_framebufferResized = false;
		m_swapchain.RecreateSwapchain(this);
	}
	else if (result != VK_SUCCESS) 
	{
		AssertMsg("failed to present swapchain image!");
	}

	m_currentFrameIndex = (m_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

}

void IVulkanRenderer::CreateVulkanInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = g_enginePtr->m_applicationName;
	appInfo.pEngineName = "NULL";	// TODO: Need engine name.
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);	// TODO: Link these with a versiongen system.
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Validation Layers
	if (enableValidationLayers && !RequestValidationLayerSupport())
	{
		AssertMsg("Validation layers requested, but not available!");
	}

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();

		LOG_INFO("Enabled Vulkan Validation Layers: ");
		for (int i = 0; i < m_validationLayers.size(); i++)
		{
			LOG_DEBUG(m_validationLayers[i]);
		}
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	// Extensions
	std::vector<const char*> extensionNames;
	extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensionNames.push_back(g_enginePtr->m_PlatformLayer->Platform_GetVulkanExtensions());

#ifdef _DEBUG
	extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();

#ifdef _DEBUG
	LOG_INFO("Enabled Vulkan Extensions: ");
	for (int i = 0; i < extensionNames.size(); i++)
	{
		LOG_DEBUG(extensionNames[i]);
	}
#endif

	// TODO: Custom allocator
	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		AssertMsg("Failed to create Vulkan instance!");
	}
}

bool IVulkanRenderer::RequestValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (int i = 0; i < m_validationLayers.size(); i++)
	{
		bool layerFound = false;

		for (int j = 0; i < availableLayers.size(); j++)
		{
			if (string_compare(m_validationLayers[i], availableLayers[j].layerName) == true)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

void IVulkanRenderer::CreateDebugMessenger()
{
#ifndef _DEBUG
	// No Vulkan Debug Callbacks in release builds!
	return;
#endif

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallbackFunc;

	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		// TODO: Custom Allocator
		VkResult result = func(m_instance, &createInfo, nullptr, &m_debugMessenger);

		if (result != VK_SUCCESS)
		{
			AssertMsg("Failed to Create Debugger!");
		}
	}
	else
	{
		LOG_FATAL("Failed to Create Debugger (VK_ERROR_EXTENSION_NOT_PRESENT)");
	}

}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallbackFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		LOG_VERBOSE(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		LOG_INFO(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LOG_WARN(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LOG_ERROR(pCallbackData->pMessage);
		break;
	}

	return VK_FALSE;
}

void IVulkanRenderer::DestroyDebugger()
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		// TODO: Custom Allocator
		func(m_instance, m_debugMessenger, nullptr);
	}
}

void IVulkanRenderer::CreateSurface()
{
	// TODO: Make platform agnostic
	VkWin32SurfaceCreateInfoKHR createInfo = g_enginePtr->m_PlatformLayer->Platform_GetVulkanSurface();

	// TODO: Custom allocator
	if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface) != VK_SUCCESS)
	{
		AssertMsg("Failed to create window surface!");
	}

	LOG_INFO("Vulkan Surface Created.");
}

void IVulkanRenderer::SetupDevice()
{
	m_device.SelectPhysicalDevice(this);

	m_device.CreateLogicalDevice(this);
}

void IVulkanRenderer::CreateSwapchain()
{
	m_swapchain.CreateSwapchain(this);

	m_swapchain.CreateImageViews(&m_device.GetLogicalDevice());

	LOG_INFO("Vulkan Swapchain created.");
}

void IVulkanRenderer::CreateRenderpass()
{
	m_renderpass.CreateRenderpass(this);

	LOG_INFO("Vulkan Renderpass created.");
}

void IVulkanRenderer::CreateGraphicsPipeline()
{
	m_pipeline.CreateGraphicsPipeline(this);

	LOG_INFO("Vulkan Pipeline created.");
}

void IVulkanRenderer::CreateFramebuffers()
{
	m_swapchain.CreateFramebuffers(this);
}

void IVulkanRenderer::CreateCommandPool()
{
	QueueFamilyIndices& queueFamilyIndices = m_device.GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(m_device.GetLogicalDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		AssertMsg("Failed to create command pool!");
	}

	LOG_INFO("Vulkan command pool created.");
}

void IVulkanRenderer::CreateCommandBuffers()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if (vkAllocateCommandBuffers(m_device.GetLogicalDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		AssertMsg("Failed to allocate command buffers!");
	}

	LOG_INFO("Vulkan command buffers created.");
}

void IVulkanRenderer::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(g_vertices[0]) * g_vertices.size();

	VulkanBuffer_t stagingBuffer;
	VulkanBuffer_t::CreateBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
		g_enginePtr->m_memorySystem->Mem_Copy(data, g_vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory());

	VulkanBuffer_t::CreateBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer);

	VulkanBuffer_t::CopyBuffer(this, stagingBuffer.GetBufferHandle(), m_vertexBuffer.GetBufferHandle(), bufferSize);

	vkDestroyBuffer(m_device.GetLogicalDevice(), stagingBuffer.GetBufferHandle(), nullptr);
	vkFreeMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory(), nullptr);
}

void IVulkanRenderer::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(g_indices[0]) * g_indices.size();

	VulkanBuffer_t stagingBuffer;
	VulkanBuffer_t::CreateBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
		g_enginePtr->m_memorySystem->Mem_Copy(data, g_indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory());

	VulkanBuffer_t::CreateBuffer(this, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer);

	VulkanBuffer_t::CopyBuffer(this, stagingBuffer.GetBufferHandle(), m_indexBuffer.GetBufferHandle(), bufferSize);

	vkDestroyBuffer(m_device.GetLogicalDevice(), stagingBuffer.GetBufferHandle(), nullptr);
	vkFreeMemory(m_device.GetLogicalDevice(), stagingBuffer.GetBufferMemory(), nullptr);
}

void IVulkanRenderer::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device.GetLogicalDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_device.GetLogicalDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_device.GetLogicalDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

bool IVulkanRenderer::RendererBackend_ShaderCreate(Shader shader)
{
	return true;
}

VkShaderModule IVulkanRenderer::CreateShaderModule(VulkanDevice_t* device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device->GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		AssertMsg("Failed to create shader module!");
	}

	return shaderModule;
}

bool WindowResizeCallback(void* data)
{
	g_framebufferResized = true;
	return true;
}

VkVertexInputBindingDescription IVulkanRenderer::GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex_t);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> IVulkanRenderer::GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex_t, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex_t, color);

	return attributeDescriptions;
}

uint32_t IVulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_device.GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	AssertMsg("Failed to find suitable memory type!");
}
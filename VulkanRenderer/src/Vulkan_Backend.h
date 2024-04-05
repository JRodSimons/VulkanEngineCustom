#pragma once

#include <vulkan/vulkan.h>

#include <EngineGlobals.h>
#include <SharedHeaders.h>

#include <optional>

#include <fstream> // TODO: TEMP

extern "C"
{
	bool VULKAN_API VulkanRendererInitialize(IEngine* enginePtr);
};

// TODO: TEMP
uint32_t GetIndexSize();

class IVulkanRenderer;

struct SwapchainSupportDetails;

//=================================
// 
// Vulkan Device
// 
//=================================
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class VulkanDevice_t
{
public:
	void				SelectPhysicalDevice(IVulkanRenderer* rendererBackend);
	void				CreateLogicalDevice(IVulkanRenderer* rendererBackend);

	VkPhysicalDevice&	GetPhysicalDevice() { return m_physicalDevice; }
	VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return m_deviceProperties; }
	VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() { return m_deviceFeatures; }
	QueueFamilyIndices&	GetQueueFamilyIndices() { return m_queueFamilyIndices; }

	SwapchainSupportDetails QueryDeviceSwapchainSupport(IVulkanRenderer* rendererBackend, VkPhysicalDevice device);

	VkDevice&			GetLogicalDevice() { return m_logicalDevice; }

	VkQueue&			GetGraphicsQueue() { return m_graphicsQueue; }
	VkQueue&			GetPresentQueue() { return m_presentQueue; }

private:
	bool				DeviceMeetsRequirements(IVulkanRenderer* rendererBackend, VkPhysicalDevice device);

	QueueFamilyIndices	FindQueueFamilyIndices(IVulkanRenderer* rendererBackend, VkPhysicalDevice device);

	bool				CheckDeviceExtensionSupport(VkPhysicalDevice device);
	bool				DeviceSupportsSwapchain(SwapchainSupportDetails details);

private:
	VkPhysicalDevice			m_physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties	m_deviceProperties;
	VkPhysicalDeviceFeatures	m_deviceFeatures;
	QueueFamilyIndices			m_queueFamilyIndices;

	VkDevice					m_logicalDevice;

	VkQueue						m_graphicsQueue;
	VkQueue						m_presentQueue;
};

//=================================
// 
// Swapchain
// 
//=================================
struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapchain_t
{
public:
	void					CreateSwapchain(IVulkanRenderer* rendererBackend);
	void					CreateImageViews(VkDevice* device);

	void					RecreateSwapchain(IVulkanRenderer* rendererBackend);
	void					CleanupSwapchain(IVulkanRenderer* rendererBackend);

	void					CreateFramebuffers(IVulkanRenderer* rendererBackend);

	VkSwapchainKHR&			GetSwapchainHandle() { return m_swapchainHandle; }
	std::vector<VkImageView>& GetSwapchainImageViews() { return m_swapchainImageViews; }

	VkFormat&				GetSwapchainImageFormat() { return m_swapchainImageFormat; }

	VkExtent2D&				GetSwapchainExtent() { return m_swapchainExtent; }

	std::vector<VkFramebuffer>& GetSwapchainFramebuffers() { return m_swapchainFramebuffers; }

private:
	VkSurfaceFormatKHR		ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR		ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D				ChooseSwapchainExtent(IVulkanRenderer* rendererBackend, const VkSurfaceCapabilitiesKHR& capabilities);

private:
	VkSwapchainKHR			m_swapchainHandle;

	std::vector<VkImage>		m_swapchainImages;
	std::vector<VkImageView>	m_swapchainImageViews;

	VkFormat				m_swapchainImageFormat;
	VkExtent2D				m_swapchainExtent;

	// Framebuffers
	std::vector<VkFramebuffer>	m_swapchainFramebuffers;
};

//=================================
// 
// Renderpass
// 
//=================================
class VulkanRenderpass_t
{
public:
	void				CreateRenderpass(IVulkanRenderer* rendererBackend);

	VkRenderPass		GetRenderpassHandle() { return m_renderpassHandle; }

private:
	VkRenderPass		m_renderpassHandle;
};

//=================================
// 
// Graphics Pipeline
// 
//=================================
class VulkanPipeline_t
{
public:
	void				CreateGraphicsPipeline(IVulkanRenderer* rendererBackend);

	VkPipeline&			GetPipelineHandle() { return m_graphicsPipeline; }
	VkPipelineLayout&	GetPipelineLayout() { return m_pipelineLayout; }

private:
	VkPipeline			m_graphicsPipeline;
	VkPipelineLayout	m_pipelineLayout;
};

//=================================
// 
// Command Buffers
// 
//=================================
class VulkanCommandBuffer_t
{
public:
	static void			RecordCommandBuffer(IVulkanRenderer* rendererBackend, VkCommandBuffer commandBuffer, uint32_t imageIndex);

private:
};

//=================================
// 
// Buffers
// 
//=================================
class VulkanBuffer_t
{
public:
	static void			CreateBuffer(IVulkanRenderer* rendererBackend, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VulkanBuffer_t& buffer);

	static void			CopyBuffer(IVulkanRenderer* rendererBackend, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkBuffer&			GetBufferHandle() { return m_bufferHandle; }
	VkDeviceMemory&		GetBufferMemory() { return m_bufferMemory; }

private:
	VkBuffer			m_bufferHandle;
	VkDeviceMemory		m_bufferMemory;
};

//=================================
// 
// Main Rendering Backend
// 
//=================================
class VULKAN_API IVulkanRenderer : public IGenericRenderer
{
public:
	IVulkanRenderer(void);
	
	bool				RendererBackend_Initialize(IEngine* enginePtr) override;
	void				RendererBackend_Update(RenderPacket_t renderPacket) override;
	void				RendererBackend_Shutdown() override;
	void				RendererBackend_DrawFrame() override;

	virtual bool		RendererBackend_ShaderCreate(Shader shader) override;

	// TODO: TEMP
	static VkShaderModule		CreateShaderModule(VulkanDevice_t* device, const std::vector<char>& code);

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	// TODO: END TEMP

	uint32_t			FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


	VkInstance& GetVulkanInstance() { return m_instance; }
	const std::vector<const char*>& GetValidationLayers() { return m_validationLayers; }

	VkSurfaceKHR&		GetVulkanSurface() { return m_surface; }
	VulkanDevice_t&		GetDevice() { return m_device; }

	VulkanSwapchain_t&	GetSwapchain() { return m_swapchain; }

	VulkanRenderpass_t& GetRenderpass() { return m_renderpass; }

	VulkanPipeline_t&	GetPipeline() { return m_pipeline; }

	VulkanBuffer_t&		GetVertexBuffer() { return m_vertexBuffer; }
	VulkanBuffer_t&		GetIndexBuffer() { return m_indexBuffer; }

	VkCommandPool&		GetCommandPool() { return m_commandPool; }

	// TODO: Can this be done some other way? This is ugly
	IEngine* m_enginePtr;

private:
	void				CreateVulkanInstance();
	void				CreateDebugMessenger();
	void				CreateSurface();
	void				SetupDevice();
	void				CreateSwapchain();
	void				CreateRenderpass();
	void				CreateGraphicsPipeline();
	void				CreateFramebuffers();
	void				CreateCommandPool();
	void				CreateVertexBuffer();
	void				CreateIndexBuffer();
	void				CreateCommandBuffers();
	void				CreateSyncObjects();

	// Instance
	bool				RequestValidationLayerSupport();

	// Debugger
	void				DestroyDebugger();

private:
	// Instance
	VkInstance			m_instance;

	// Debug Messenger
	VkDebugUtilsMessengerEXT	m_debugMessenger;

	// Surface
	VkSurfaceKHR		m_surface;

	// Vulkan Device
	VulkanDevice_t		m_device;

	// Swapchain
	VulkanSwapchain_t	m_swapchain;

	// Renderpass(es)
	VulkanRenderpass_t	m_renderpass;

	// Pipeline
	VulkanPipeline_t	m_pipeline;

	// Command buffers
	VkCommandPool		m_commandPool;
	std::vector<VkCommandBuffer>		m_commandBuffers;

	// Buffers
	VulkanBuffer_t		m_vertexBuffer;
	VulkanBuffer_t		m_indexBuffer;

	// Sync Objects
	std::vector<VkSemaphore>			m_imageAvailableSemaphores;
	std::vector<VkSemaphore>			m_renderFinishedSemaphores;
	std::vector<VkFence>				m_inFlightFences;

	unsigned int						m_currentFrameIndex = 0;

	

	const std::vector<const char*> m_validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

public:
#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif
};



// TODO: SUPER TEMP! Integrate with shader system eventually.
static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
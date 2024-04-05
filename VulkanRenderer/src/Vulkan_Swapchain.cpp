#define NOMINMAX	// For numeric limits
#include "Vulkan_Backend.h"

#include "Platform/Platform.h"

#include <limits>
#include <algorithm>

void VulkanSwapchain_t::CreateSwapchain(IVulkanRenderer* rendererBackend)
{
	SwapchainSupportDetails swapchainSupport = rendererBackend->GetDevice().QueryDeviceSwapchainSupport(rendererBackend, rendererBackend->GetDevice().GetPhysicalDevice());

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = ChooseSwapchainExtent(rendererBackend, swapchainSupport.capabilities);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

	if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
	{
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = rendererBackend->GetVulkanSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices& indices = rendererBackend->GetDevice().GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// TODO: Custom allocator.
	if (vkCreateSwapchainKHR(rendererBackend->GetDevice().GetLogicalDevice(), &createInfo, nullptr, &m_swapchainHandle) != VK_SUCCESS)
	{
		AssertMsg("Failed to create swapchain!");
	}

	vkGetSwapchainImagesKHR(rendererBackend->GetDevice().GetLogicalDevice(), m_swapchainHandle, &imageCount, nullptr);
	m_swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(rendererBackend->GetDevice().GetLogicalDevice(), m_swapchainHandle, &imageCount, m_swapchainImages.data());
	m_swapchainImageFormat = surfaceFormat.format;
	m_swapchainExtent = extent;
}

void VulkanSwapchain_t::CreateImageViews(VkDevice* device)
{
	m_swapchainImageViews.resize(m_swapchainImages.size());

	for (int i = 0; i < m_swapchainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapchainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(*device, &createInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS)
		{
			AssertMsg("Failed to create swapchain image views!");
		}
	}
}

void VulkanSwapchain_t::RecreateSwapchain(IVulkanRenderer* rendererBackend)
{
	vkDeviceWaitIdle(rendererBackend->GetDevice().GetLogicalDevice());

	CleanupSwapchain(rendererBackend);

	CreateSwapchain(rendererBackend);
	CreateImageViews(&rendererBackend->GetDevice().GetLogicalDevice());
	CreateFramebuffers(rendererBackend);
}

void VulkanSwapchain_t::CleanupSwapchain(IVulkanRenderer* rendererBackend)
{
	for (unsigned int i = 0; i < m_swapchainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(rendererBackend->GetDevice().GetLogicalDevice(), m_swapchainFramebuffers[i], nullptr);
	}

	for (unsigned int i = 0; i < m_swapchainImageViews.size(); i++)
	{
		vkDestroyImageView(rendererBackend->GetDevice().GetLogicalDevice(), m_swapchainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(rendererBackend->GetDevice().GetLogicalDevice(), m_swapchainHandle, nullptr);
}

void VulkanSwapchain_t::CreateFramebuffers(IVulkanRenderer* rendererBackend)
{
	std::vector<VkImageView> swapchainImageViews = GetSwapchainImageViews();

	m_swapchainFramebuffers.resize(swapchainImageViews.size());

	for (unsigned int i = 0; i < swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] =
		{
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = rendererBackend->GetRenderpass().GetRenderpassHandle();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapchainExtent.width;
		framebufferInfo.height = m_swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(rendererBackend->GetDevice().GetLogicalDevice(), &framebufferInfo, nullptr, &m_swapchainFramebuffers[i]) != VK_SUCCESS)
		{
			AssertMsg("Failed to create framebuffer!");
		}
	}

	LOG_INFO("Vulkan Framebuffers created.");
}

VkSurfaceFormatKHR VulkanSwapchain_t::ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (int i = 0; i < availableFormats.size(); i++)
	{
		if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormats[i];
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain_t::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (int i = 0; i < availablePresentModes.size(); i++)
	{
		if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentModes[i];
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain_t::ChooseSwapchainExtent(IVulkanRenderer* rendererBackend, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		unsigned int width, height;
		rendererBackend->m_enginePtr->m_PlatformLayer->Platform_GetWindowSize(&width, &height);

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
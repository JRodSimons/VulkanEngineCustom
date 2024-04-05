#include "Vulkan_Backend.h"

#include <set>
#include "Core/String.h"

void VulkanDevice_t::SelectPhysicalDevice(IVulkanRenderer* rendererBackend)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(rendererBackend->GetVulkanInstance(), &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		AssertMsg("Failed to find a GPU with Vulkan support!");
		return;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(rendererBackend->GetVulkanInstance(), &deviceCount, devices.data());

	for (int i = 0; i < devices.size(); i++)
	{
		if (DeviceMeetsRequirements(rendererBackend, devices[i]))
		{
			m_physicalDevice = devices[i];
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		AssertMsg("Failed to find a suitable GPU!");
	}
	else
	{
		LOG_INFO("Selected GPU: %s", m_deviceProperties.deviceName);
	}
}

bool VulkanDevice_t::DeviceMeetsRequirements(IVulkanRenderer* rendererBackend, VkPhysicalDevice device)
{
	// Check current device that it meets minimum requirements.
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = FindQueueFamilyIndices(rendererBackend, device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	// Query device swapchain support
	SwapchainSupportDetails swapchainSupport = QueryDeviceSwapchainSupport(rendererBackend, device);
	bool swapchainSupported = DeviceSupportsSwapchain(swapchainSupport);

	// The GPU MUST be a discrete graphics card and have an available queue for graphics processing and have swapchain support.
	bool deviceMeetsRequirements = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices.isComplete() && extensionsSupported && swapchainSupported;

	if (deviceMeetsRequirements)
	{
		// Store the features and properties in the member variables.
		m_deviceProperties = deviceProperties;
		m_deviceFeatures = deviceFeatures;
		m_queueFamilyIndices = indices;
	}
	
	return deviceMeetsRequirements;
}

QueueFamilyIndices VulkanDevice_t::FindQueueFamilyIndices(IVulkanRenderer* rendererBackend, VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			// Found a suitable graphics queue
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, rendererBackend->GetVulkanSurface(), &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}
	}

	return indices;
}

bool VulkanDevice_t::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	bool extensionFound = false;

	for (int i = 0; i < availableExtensions.size(); i++)
	{
		// Device Swapchain Extension
		if (string_compare(availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == true)
		{
			extensionFound = true;
		}
	}

	if (!extensionFound)
	{
		return false;
	}

	return true;
}

void VulkanDevice_t::CreateLogicalDevice(IVulkanRenderer* rendererBackend)
{
	QueueFamilyIndices& indices = m_queueFamilyIndices;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// Required Device Extensions
	const std::vector<const char*> requiredDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Set up the device queues
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) 
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &m_deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

	if (rendererBackend->enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(rendererBackend->GetValidationLayers().size());
		createInfo.ppEnabledLayerNames = rendererBackend->GetValidationLayers().data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	// TODO: Custom allocator
	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
	{
		AssertMsg("Failed to create logical device!");
	}

	LOG_INFO("Vulkan Logical Device created.");

	vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
	LOG_INFO("Graphics Queue Index: %d", m_queueFamilyIndices.graphicsFamily.value());
	LOG_INFO("Present Queue Index: %d", m_queueFamilyIndices.presentFamily.value());
}

SwapchainSupportDetails VulkanDevice_t::QueryDeviceSwapchainSupport(IVulkanRenderer* rendererBackend, VkPhysicalDevice device)
{
	SwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, rendererBackend->GetVulkanSurface(), &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, rendererBackend->GetVulkanSurface(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, rendererBackend->GetVulkanSurface(), &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, rendererBackend->GetVulkanSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, rendererBackend->GetVulkanSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool VulkanDevice_t::DeviceSupportsSwapchain(SwapchainSupportDetails details)
{
	return !details.formats.empty() && !details.presentModes.empty();
}
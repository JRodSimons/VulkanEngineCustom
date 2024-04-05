#include "Vulkan_Backend.h"

void VulkanBuffer_t::CreateBuffer(IVulkanRenderer* rendererBackend, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VulkanBuffer_t& buffer)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(rendererBackend->GetDevice().GetLogicalDevice(), &bufferInfo, nullptr, &buffer.GetBufferHandle()) != VK_SUCCESS)
	{
		AssertMsg("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(rendererBackend->GetDevice().GetLogicalDevice(), buffer.GetBufferHandle(), &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = rendererBackend->FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(rendererBackend->GetDevice().GetLogicalDevice(), &allocInfo, nullptr, &buffer.GetBufferMemory()) != VK_SUCCESS)
	{
		AssertMsg("Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(rendererBackend->GetDevice().GetLogicalDevice(), buffer.GetBufferHandle(), buffer.GetBufferMemory(), 0);
}

void VulkanBuffer_t::CopyBuffer(IVulkanRenderer* rendererBackend, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = rendererBackend->GetCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(rendererBackend->GetDevice().GetLogicalDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(rendererBackend->GetDevice().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(rendererBackend->GetDevice().GetGraphicsQueue());

	vkFreeCommandBuffers(rendererBackend->GetDevice().GetLogicalDevice(), rendererBackend->GetCommandPool(), 1, &commandBuffer);
}
#include "Vulkan_Backend.h"

void VulkanCommandBuffer_t::RecordCommandBuffer(IVulkanRenderer* rendererBackend, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		AssertMsg("Failed to begin recording command buffer!");
	}

	std::vector<VkFramebuffer>& framebuffers = rendererBackend->GetSwapchain().GetSwapchainFramebuffers();

	VkRenderPassBeginInfo renderpassInfo{};
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpassInfo.renderPass = rendererBackend->GetRenderpass().GetRenderpassHandle();
	renderpassInfo.framebuffer = rendererBackend->GetSwapchain().GetSwapchainFramebuffers()[imageIndex];
	renderpassInfo.renderArea.offset = { 0, 0 };
	renderpassInfo.renderArea.extent = rendererBackend->GetSwapchain().GetSwapchainExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderpassInfo.clearValueCount = 1;
	renderpassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rendererBackend->GetPipeline().GetPipelineHandle());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rendererBackend->GetSwapchain().GetSwapchainExtent().width);
		viewport.height = static_cast<float>(rendererBackend->GetSwapchain().GetSwapchainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = rendererBackend->GetSwapchain().GetSwapchainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { rendererBackend->GetVertexBuffer().GetBufferHandle() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, rendererBackend->GetIndexBuffer().GetBufferHandle(), 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(GetIndexSize()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		AssertMsg("Failed to record command buffer!");
	}
}
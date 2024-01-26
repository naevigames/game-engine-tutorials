#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

#include <vulkan/vulkan.h>

#include "vk/instance.hpp"
#include "vk/surface.hpp"
#include "vk/device.hpp"
#include "vk/queue.hpp"
#include "vk/swapchain.hpp"
#include "vk/command_pool.hpp"

int32_t main()
{
    glfw::PlatformFactory platform_factory;

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    window_manager.init(&platform_factory, { "chapter_2_tutorial_1_vulkan_next", { 800, 600 }, true });

    vk::Instance instance;
    instance.create();

    vk::Surface surface;
    surface.create(instance, window_manager.handle());

    auto physical_device = instance.find_device();
         physical_device.get_queue_indices(surface);
         physical_device.get_surface_details(surface);

    vk::Device device;
    device.create(physical_device);

    auto graphics_queue = device.get_queue(physical_device.queue_indices.graphics());
    auto present_queue  = device.get_queue(physical_device.queue_indices.present());

    vk::Swapchain swapchain;
    swapchain.create(device, physical_device, surface);
    swapchain.get_images(device, physical_device.surface_details.current_format.format);

    std::vector<VkCommandBuffer> vk_command_buffers(swapchain.images_count);

    vk::CommandPool command_pool;
    command_pool.create(device, physical_device.queue_indices.graphics());

    VkCommandBufferAllocateInfo vk_command_buffer_allocate_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool._handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = swapchain.images_count
    };

    vkAllocateCommandBuffers(device._handle, &vk_command_buffer_allocate_info, vk_command_buffers.data());

    VkCommandBufferBeginInfo vk_command_begin_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
    };

    VkClearColorValue vk_clear_color { 0.5f, 0.5f, 0.5f, 1.0f };
    VkClearValue      vk_clear_value
    {
        .color = vk_clear_color
    };

    VkImageSubresourceRange vk_image_range
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1
    };

    for (uint32_t i = 0; i < vk_command_buffers.size(); i++)
    {
        vkBeginCommandBuffer(vk_command_buffers[i], &vk_command_begin_info);

        vkCmdClearColorImage(vk_command_buffers[i], swapchain.image(i), VK_IMAGE_LAYOUT_GENERAL, &vk_clear_color, 1, &vk_image_range);

        vkEndCommandBuffer(vk_command_buffers[i]);
    }

    while (window_manager.is_active())
    {
        uint32_t image_index = 0;
        vkAcquireNextImageKHR(device._handle, swapchain._handle, UINT64_MAX, nullptr, nullptr, &image_index);

        graphics_queue.submit(vk_command_buffers[image_index]);
        graphics_queue.present(swapchain, image_index);

        platform_manager.update();
    }

    command_pool.destroy(device);

    swapchain.destroy(device);
    device.destroy();

    surface.destroy(instance);
    instance.destroy();

    window_manager.release();
    platform_manager.release();

    return 0;
}
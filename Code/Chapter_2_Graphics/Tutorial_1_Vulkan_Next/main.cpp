#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

#include <vulkan/vulkan.h>

#include "vk/instance.hpp"
#include "vk/surface.hpp"
#include "vk/physical_device.hpp"
#include "vk/device.hpp"
#include "vk/queue.hpp"
#include "vk/image_view.hpp"
#include "vk/swapchain.hpp"

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

    VkCommandPool  vk_command_pool;

    vk::Instance instance;
    instance.create();

    vk::Surface surface;
    surface.create(instance, window_manager.handle());

    vk::PhysicalDevice physical_device;

    physical_device.find_device(instance);
    physical_device.find_queue(surface);

    vk::Device device;
    device.create(physical_device);

    vk::Queue graphics_queue;
    vk::Queue present_queue;

    graphics_queue.find_queue(device, physical_device.queue_indices.graphics());
    present_queue.find_queue(device,  physical_device.queue_indices.present());

    VkSurfaceCapabilitiesKHR vk_surface_capabilities;
    std::vector<VkSurfaceFormatKHR> vk_surface_formats;
    std::vector<VkPresentModeKHR>   vk_present_modes;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device._handle, surface._handle, &vk_surface_capabilities);

    uint32_t vk_surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device._handle, surface._handle, &vk_surface_format_count, nullptr);

    vk_surface_formats.resize(vk_surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device._handle, surface._handle, &vk_surface_format_count, vk_surface_formats.data());

    uint32_t vk_present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device._handle, surface._handle, &vk_present_mode_count, nullptr);

    vk_present_modes.resize(vk_present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device._handle, surface._handle, &vk_present_mode_count, vk_present_modes.data());

    VkSurfaceFormatKHR vk_available_format = vk_surface_formats[0];

    vk::Swapchain swapchain;
    swapchain.create(device, surface, { vk_surface_capabilities, vk_available_format }, physical_device.queue_indices);

    uint32_t local_swapchain_images_count;
    vkGetSwapchainImagesKHR(device._handle, swapchain._handle, &local_swapchain_images_count, nullptr);

    std::vector<VkImage> vk_swapchain_images(local_swapchain_images_count);
    vkGetSwapchainImagesKHR(device._handle, swapchain._handle, &local_swapchain_images_count, vk_swapchain_images.data());

    std::vector<vk::ImageView> swapchain_images_views;
    swapchain_images_views.resize(local_swapchain_images_count);

    for (int32_t i = 0; i < local_swapchain_images_count; i++)
    {
        swapchain_images_views[i].create(device, vk_swapchain_images[i], vk_available_format.format);
    }

    std::vector<VkCommandBuffer> vk_command_buffers(local_swapchain_images_count);

    VkCommandPoolCreateInfo vk_command_pool_create_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = physical_device.queue_indices.graphics(),
    };

    vkCreateCommandPool(device._handle, &vk_command_pool_create_info, nullptr, &vk_command_pool);

    VkCommandBufferAllocateInfo vk_command_buffer_allocate_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = local_swapchain_images_count
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

        vkCmdClearColorImage(vk_command_buffers[i], vk_swapchain_images[i], VK_IMAGE_LAYOUT_GENERAL, &vk_clear_color, 1, &vk_image_range);

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

    for (auto image_view : swapchain_images_views)
    {
        image_view.destroy(device);
    }

    swapchain.destroy(device);
    device.destroy();

    surface.destroy(instance);
    instance.destroy();

    window_manager.release();
    platform_manager.release();

    return 0;
}
#include "platform_manager.hpp"
#include "window_manager.hpp"

#include "glfw/platform_factory.hpp"

#include <glfw/glfw3.h>

#include <vulkan/vulkan.h>

#include <set>

#include "vk/instance.hpp"
#include "vk/surface.hpp"
#include "vk/physical_device.hpp"

int32_t main()
{
    glfw::PlatformFactory platform_factory;

    auto& platform_manager = PlatformManager::instance();
    auto& window_manager   = WindowManager::instance();

    if (!platform_manager.init(&platform_factory))
    {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_manager.init(&platform_factory, { "chapter_2_tutorial_1_vulkan_next", { 800, 600 } });

    VkDevice       vk_device;
    VkCommandPool  vk_command_pool;
    VkQueue        vk_graphics_queue;
    VkQueue        vk_present_queue;
    VkSwapchainKHR vk_swapchain;

    vk::Instance instance;
    instance.create();

    vk::Surface surface;
    surface.create(instance, window_manager.handle());

    vk::PhysicalDevice physical_device;

    physical_device.find_device(instance);
    physical_device.find_queue(surface);

    float vk_queue_priority = 1.0f;
    std::array<const char*, 1> vk_device_extensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<VkDeviceQueueCreateInfo>            vk_queue_create_infos;
    std::set<uint32_t> vk_unique_queue_families = { physical_device.graphics_queue_index, physical_device.present_queue_index };

    for (uint32_t queue_family : vk_unique_queue_families)
    {
        vk_queue_create_infos.push_back({
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_family,
            .queueCount       = 1,
            .pQueuePriorities = &vk_queue_priority
        });
    }

    VkPhysicalDeviceFeatures vk_device_features
    {
    };
    VkDeviceCreateInfo       vk_device_create_info
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount    = static_cast<uint32_t>(vk_queue_create_infos.size()),
        .pQueueCreateInfos       = vk_queue_create_infos.data(),
        .enabledExtensionCount   = (int32_t)vk_device_extensions.size(),
        .ppEnabledExtensionNames = vk_device_extensions.data(),
        .pEnabledFeatures        = &vk_device_features
    };

    vkCreateDevice(physical_device._handle, &vk_device_create_info, nullptr, &vk_device);

    vkGetDeviceQueue(vk_device, physical_device.graphics_queue_index, 0, &vk_graphics_queue);
    vkGetDeviceQueue(vk_device, physical_device.present_queue_index, 0, &vk_present_queue);

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
    VkPresentModeKHR   vk_available_mode   = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t vk_swapchain_images_count = vk_surface_capabilities.minImageCount + 1;
    uint32_t local_family_indices[2]   =
    {
        physical_device.graphics_queue_index,
        physical_device.present_queue_index
    };

    VkSwapchainCreateInfoKHR vk_swapchain_create_info
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface._handle,
        .minImageCount = vk_swapchain_images_count,
        .imageFormat = vk_available_format.format,
        .imageColorSpace = vk_available_format.colorSpace,
        .imageExtent = vk_surface_capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = vk_surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = vk_available_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if (vk_unique_queue_families.size() == 1)
    {
        vk_swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else
    {
        vk_swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        vk_swapchain_create_info.queueFamilyIndexCount = 2,
        vk_swapchain_create_info.pQueueFamilyIndices   = local_family_indices;
    }

    vkCreateSwapchainKHR(vk_device, &vk_swapchain_create_info, nullptr, &vk_swapchain);

    uint32_t local_swapchain_images_count;
    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &local_swapchain_images_count, nullptr);

    std::vector<VkImage> vk_swapchain_images(local_swapchain_images_count);
    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &local_swapchain_images_count, vk_swapchain_images.data());

    std::vector<VkImageView> vk_swapchain_images_view;
    vk_swapchain_images_view.resize(local_swapchain_images_count);

    for (int32_t i = 0; i < local_swapchain_images_count; i++)
    {
        VkImageViewCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = vk_swapchain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vk_available_format.format,
            .components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
        };

        vkCreateImageView(vk_device, &create_info, nullptr, &vk_swapchain_images_view[i]);
    }

    std::vector<VkCommandBuffer> vk_command_buffers(local_swapchain_images_count);

    VkCommandPoolCreateInfo vk_command_pool_create_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = physical_device.graphics_queue_index,
    };

    vkCreateCommandPool(vk_device, &vk_command_pool_create_info, nullptr, &vk_command_pool);

    VkCommandBufferAllocateInfo vk_command_buffer_allocate_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = local_swapchain_images_count
    };

    vkAllocateCommandBuffers(vk_device, &vk_command_buffer_allocate_info, vk_command_buffers.data());

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
        uint32_t local_image_index = 0;
        vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, nullptr, nullptr, &local_image_index);

        VkSubmitInfo vk_submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &vk_command_buffers[local_image_index]
        };

        vkQueueSubmit(vk_graphics_queue, 1, &vk_submit_info, nullptr);

        VkPresentInfoKHR vk_present_info
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains    = &vk_swapchain,
            .pImageIndices  = &local_image_index
        };

        vkQueuePresentKHR(vk_graphics_queue, &vk_present_info);

        platform_manager.update();
    }

    for (auto image_view : vk_swapchain_images_view)
    {
        vkDestroyImageView(vk_device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);

    vkDestroyDevice(vk_device, nullptr);

    surface.destroy(instance);
    instance.destroy();

    window_manager.release();
    platform_manager.release();

    return 0;
}
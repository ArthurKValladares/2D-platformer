#pragma once

namespace initializers {
    inline VkImageCreateInfo image_create_info()
    {
        return VkImageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO
        };
    }
};
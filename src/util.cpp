#include "util.h"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

void chk(VkResult result) {
    if (result != VK_SUCCESS) {
        std::cerr << "Vulkan call returned an error\n";
        exit(result);
    }
}
void chk(bool result) {
    if (!result) {
        std::cerr << "Call returned an error\n";
        exit(result);
    }
}

void chk_sdl(bool result) {
    if (!result) {
        std::cerr << SDL_GetError() << std::endl;
        exit(result);
    }
}

Radians Degrees::to_radians() const {
    return Radians(glm::radians(val));
}

Degrees Radians::to_degrees() const {
    return Degrees(glm::degrees(val));
}
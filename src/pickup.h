#pragma once

#include <functional>

#include "rect.h"
#include "renderables/includes.h"
#include "assets.h"

struct Pickup {
    Pickup() {}

    Renderable draw();

    Rect2D location;
    TextureSource texture;
    glm::vec4 color;
};

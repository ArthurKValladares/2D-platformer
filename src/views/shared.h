#pragma once

#include "../renderer/renderer.h"

struct ViewUpdateData {
    Renderer* renderer;
    double elapsed_seconds;
};
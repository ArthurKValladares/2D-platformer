#pragma once

#include "../renderer/renderer.h"
#include "../keyboard_state.h"

struct ViewUpdateData {
    Renderer* renderer;
    double elapsed_seconds;
    const KeyboardState& keyboard_state;
};
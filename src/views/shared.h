#pragma once

#include "../renderer/renderer.h"
#include "../keyboard_state.h"

struct ViewUpdateData {
    Renderer* renderer;
    double total_elapsed_seconds;
    double frame_dt;
    const KeyboardState& keyboard_state;
};
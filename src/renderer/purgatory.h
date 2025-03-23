#pragma once

#include <array>
#include <vector>

#include "tools.h"
#include "buffer.h"

struct Renderer;
struct Purgatory {
    void destroy(Renderer* renderer);

    std::array<std::vector<Buffer>, MAX_FRAMES_IN_FLIGHT> buffers;
};
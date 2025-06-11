#pragma once

#include "imgui.h"

struct ImguiLog {
    ImguiLog()
        : open(false)
        , scroll_to_bottom(false)
    {}

    bool open;
    ImGuiTextBuffer buf;
    ImGuiTextFilter filter;
    ImVector<int> idx_line_offsets;
    bool scroll_to_bottom;

    void clear();
    void add_log(const char* fmt, ...) IM_FMTARGS(2);
    void draw();
};
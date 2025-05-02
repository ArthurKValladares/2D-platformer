#pragma once

#include "map_editor/tile_types.h"

#include "imgui.h"

struct MapEditor {
    MapEditor() {}

    void imgui_node() {
        ImGuiIO& io = ImGui::GetIO();
        const ImTextureID my_tex_id = io.Fonts->TexID;
        const float my_tex_w = (float)io.Fonts->TexWidth;
        const float my_tex_h = (float)io.Fonts->TexHeight;
        const ImVec2 size = ImVec2(32.0f, 32.0f);
        const ImVec2 uv0 = ImVec2(0.0f, 0.0f);
        const ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);
        const ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::ImageButton("Wall", my_tex_id, size, uv0, uv1, bg_col, tint_col);
    }
};
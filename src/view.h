#pragma once

#include "keyboard_state.h"
#include "logger.h"
#include "renderer/renderer.h"
#include "renderables/includes.h"

// TODO: Context structs

struct View {
    virtual const char* name() const = 0;
    virtual void update_fn(const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt) = 0;
    virtual ViewDrawData draw_fn(Renderer* renderer, Renderable* renderable, double total_elapsed_time) = 0;
    virtual void draw_imgui(ImguiLog& logger, double total_elapsed_time) = 0;
    virtual void cleanup(Renderer* renderer) = 0;
};
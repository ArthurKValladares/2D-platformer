#pragma once

#include "keyboard_state.h"
#include "logger.h"
#include "ui.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

struct UpdateContext {
    Window& window;
    KeyboardState& keyboard_state;
    MouseState& mouse_state;
    UI& ui;
};

struct View {
    virtual const char* name() const = 0;
    virtual void update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) = 0;
    virtual RootRenderable draw_fn(Renderer* renderer, double total_elapsed_time) = 0;
    // TODO: This should really be in app and not a part of view. And its not really `draw_ui`, its about the pause menu
    virtual bool should_draw_ui() const = 0;
    virtual RootRenderable draw_ui(UI& ui, Renderer* renderer, double total_elapsed_time) = 0;
    virtual void draw_imgui(ImguiLog& logger, double total_elapsed_time) = 0;
    virtual void cleanup(Renderer* renderer) = 0;
};
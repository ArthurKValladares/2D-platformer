#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

#include "renderer/renderer.h"
#include "renderables/includes.h"

#include "animatable.h"
#include "assets.h"
#include "util.h"

#include <iostream>

template<class T>
struct InterpolatableField {
    InterpolatableField()
    {}
    InterpolatableField(T start, T end = start)
        : start(start)
        , end(end)
    {}

    T start;
    T end;
};

template<class T>
struct VariableField {
    VariableField()
    {}
    VariableField(T base_val, T variability = {})
        : base_val(base_val)
        , variability(variability)
    {}

    T base_val;
    T variability;
};

inline float get_variable_float_val(const VariableField<float>& var_float) {
    const float curr_variability = random_num_in_range(-var_float.variability, var_float.variability);
    return var_float.base_val + curr_variability;
}

inline bool imgui_variable_float(const char* name, VariableField<float>& var_float, int& id) {
    bool has_changed = false;
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat("##", &var_float.base_val);
    ImGui::PopID();
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat("##", &var_float.variability);
    ImGui::PopID();
    return has_changed;
}

inline Degrees get_variable_degrees_val(const VariableField<Degrees>& var_degrees) {
    const float curr_variability = random_num_in_range(-var_degrees.variability.val, var_degrees.variability.val);
    return Degrees(var_degrees.base_val.val + curr_variability);
}

inline bool imgui_variable_degrees(const char* name, VariableField<Degrees>& var, int& id) {
    bool has_changed = false;
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat("##", &var.base_val.val);
    ImGui::PopID();
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat("##", &var.variability.val);
    ImGui::PopID();
    return has_changed;
}

inline glm::vec2 get_variable_vec2_val(const VariableField<glm::vec2>& var_size) {
    const float size_scale = random_num_in_range(0.0, 1.0);
    const glm::vec2 size_var = glm::vec2(
        lerp(-var_size.variability.x, var_size.variability.x, size_scale),
        lerp(-var_size.variability.y, var_size.variability.y, size_scale)
    );

    return var_size.base_val + size_var;
}

inline glm::vec2 get_variable_vec2_val_independent(const VariableField<glm::vec2>& var_size) {
    const glm::vec2 size_var = glm::vec2(
        lerp(-var_size.variability.x, var_size.variability.x, random_num_in_range(0.0f, 1.0f)),
        lerp(-var_size.variability.y, var_size.variability.y, random_num_in_range(0.0f, 1.0f))
    );

    return var_size.base_val + size_var;
}

inline bool imgui_variable_vec2(const char* name, VariableField<glm::vec2>& var, int& id) {
    bool has_changed = false;
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat2("##", glm::value_ptr(var.base_val));
    ImGui::PopID();
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed |= ImGui::DragFloat2("##", glm::value_ptr(var.variability));
    ImGui::PopID();
    return has_changed;
}

inline glm::vec3 get_variable_vec3_val(const VariableField<glm::vec3>& var_color) {
    const float color_scale = random_num_in_range(0.0, 1.0);
    const glm::vec3 color_var = glm::vec3(
        lerp(-var_color.variability.x, var_color.variability.x, color_scale),
        lerp(-var_color.variability.y, var_color.variability.y, color_scale),
        lerp(-var_color.variability.z, var_color.variability.z, color_scale)
    );

    return var_color.base_val + color_var;
}

inline bool imgui_variable_color(const char* name, VariableField<glm::vec3>& var, int& id) {
    bool has_changed = false;
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed = has_changed || ImGui::ColorEdit3("##", glm::value_ptr(var.base_val));
    ImGui::PopID();
    ImGui::TableNextColumn();
    ImGui::PushID(id++);
    has_changed = has_changed || ImGui::ColorEdit3("##", glm::value_ptr(var.variability));
    ImGui::PopID();
    return has_changed;
}

struct Particle {
    Particle(double start_time, double duration, glm::vec2 pos, glm::vec2 dir, float vel, glm::vec2 start_size, glm::vec2 end_size, glm::vec3 start_color, glm::vec3 end_color) 
        : pos(pos)
        , dir(dir)
        , vel(vel)
        , start_size(start_size)
        , end_size(end_size)
        , start_color(start_color)
        , end_color(end_color)
        , anim(AnimatableFloat(0.0, 1.0, duration, start_time))
    {}

    RenderableRect get_quad(double curr_time, TextureSource texture) const {
        const double elapsed_time = curr_time - anim.start_time;
        const float anim_at = (float) anim.value_at(curr_time);
        
        const glm::vec2 curr_pos = pos + dir * glm::vec2(vel * elapsed_time);
        const glm::vec2 curr_size = glm::vec2(
            lerp(start_size.x, end_size.x, anim_at),
            lerp(start_size.y, end_size.y, anim_at)
        );
        const glm::vec3 curr_color = glm::vec3(
            lerp(start_color.r, end_color.r, anim_at),
            lerp(start_color.g, end_color.g, anim_at),
            lerp(start_color.b, end_color.b, anim_at)
        );

        return colored_quad(
            Rect2D(curr_pos, curr_size),
            texture_id(texture),
            curr_color
        );
    }

    glm::vec2 pos;
    glm::vec2 dir;
    float vel;
    glm::vec2 start_size;
    glm::vec2 end_size;
    glm::vec3 start_color;
    glm::vec3 end_color;
    AnimatableFloat anim;
};

struct EmitterLifetime {
    EmitterLifetime()
        : val(-1.f)
    {}
    EmitterLifetime(float lifetime)
        : val(lifetime)
    {}

    static EmitterLifetime infinite() {
        return EmitterLifetime();
    }

    bool is_infinite() const {
        return val < 0.0;
    }

    float val;
};

struct ParticleEmitter {
    ParticleEmitter()
    {}

    ParticleEmitter(
        double start_time,
        EmitterLifetime lifetime, 
        glm::vec2 center,
        VariableField<glm::vec2> start_offset,
        VariableField<float> emission_delay,
        VariableField<Degrees> emission_angle,
        VariableField<float> particle_lifetime,
        VariableField<float> particle_vel,
        InterpolatableField<VariableField<glm::vec2>> size,
        InterpolatableField<VariableField<glm::vec3>> color,
        TextureSource texture
    )
        : start_time(start_time)
        , last_updated(start_time)
        , lifetime(lifetime)
        , center(center)
        , start_offset(start_offset)
        , emission_delay(emission_delay)
        , emission_angle(emission_angle)
        , particle_lifetime(particle_lifetime)
        , particle_vel(particle_vel)
        , size(size)
        , color(color)
        , texture(texture)
        , selected_texture(static_cast<int>(texture))
    {}

    bool is_alive_at(double curr_time) const {
        return (curr_time - start_time) <= lifetime.val;
    }

    void update(double curr_time) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), 
            [curr_time](Particle& p) {
                return p.anim.is_done_at(curr_time);
            }),
            particles.end()
        );

        if (texture != TextureSource::Count &&
            (lifetime.is_infinite() || is_alive_at(curr_time))
        ) {
            const float c_emission_delay = get_variable_float_val(emission_delay);
            if (curr_time - last_updated > c_emission_delay) {
                const glm::vec2 c_start_offset      = get_variable_vec2_val_independent(start_offset);
                const float     c_particle_duration = get_variable_float_val(particle_lifetime);
                const Radians   c_particle_angle    = get_variable_degrees_val(emission_angle).to_radians();
                const glm::vec2 c_particle_dir      = glm::vec2(cos(c_particle_angle.val), sin(c_particle_angle.val));
                const float     c_velocity          = get_variable_float_val(particle_vel);
                const glm::vec2 c_start_size        = get_variable_vec2_val(size.start);
                const glm::vec2 c_end_size          = get_variable_vec2_val(size.end);
                const glm::vec3 c_start_color       = get_variable_vec3_val(color.start);
                const glm::vec3 c_end_color         = get_variable_vec3_val(color.end);

                particles.push_back(Particle(curr_time, c_particle_duration, center + c_start_offset, c_particle_dir, c_velocity, c_start_size, c_end_size, c_start_color, c_end_color));
                last_updated = curr_time;
            }
        }
    }

    Renderable draw(double curr_time) const {
        Renderable root;
        for (const Particle& particle : particles) {
            root.push_child(Renderable(particle.get_quad(curr_time, texture)));
        }
        return root;
    }

    bool imgui_node() {
        bool has_changed = false;

        has_changed |= ImGui::DragFloat("lifetime", &lifetime.val);
        
        if (ImGui::BeginTable("Values", 3)) {

            ImGui::TableSetupColumn("Field");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Variability");
            ImGui::TableHeadersRow();

            int id = 0;
            has_changed |= imgui_variable_vec2("Starting offset", start_offset, id);
            has_changed |= imgui_variable_float("Emission Delay", emission_delay, id);
            has_changed |= imgui_variable_degrees("Emission Angle", emission_angle, id);
            has_changed |= imgui_variable_float("Particle Lifetime", particle_lifetime, id);
            has_changed |= imgui_variable_float("Particle Velocity", particle_vel, id);
            has_changed |= imgui_variable_vec2("Start Size", size.start, id);
            has_changed |= imgui_variable_vec2("End Size", size.end, id);
            has_changed |= imgui_variable_color("Start Color", color.start, id);
            has_changed |= imgui_variable_color("End Color", color.end, id);

            ImGui::EndTable();
        }

        const char** textures = texture_names();
        const bool check =  ImGui::Combo("Texture", &selected_texture, textures, texture_count());
        if (check) {
            texture = static_cast<TextureSource>(selected_texture);
        }

        return has_changed;
    }

    double start_time;
    double last_updated;

    EmitterLifetime lifetime;

    glm::vec2 center;

    VariableField<glm::vec2> start_offset;
    VariableField<float> emission_delay;
    VariableField<Degrees> emission_angle;
    VariableField<float> particle_lifetime;
    VariableField<float> particle_vel;
    InterpolatableField<VariableField<glm::vec2>> size;
    InterpolatableField<VariableField<glm::vec3>> color;

    TextureSource texture;
    int selected_texture;

    std::vector<Particle> particles;
};
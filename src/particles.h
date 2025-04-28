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

#define FLOAT_ROW(name, field) do {\
    ImGui::TableNextColumn();\
    ImGui::Text(name);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat("##"#field, &field);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat("##"#field"_var", &field ## _var);\
} while(0)

#define FLOAT_ROW_ACCESSOR(name, field, acessor) do {\
    ImGui::TableNextColumn();\
    ImGui::Text(name);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat("##"#field".val", &field.val);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat("##"#field"_var.val", &field ## _var.val);\
} while(0)

#define FLOAT_ROW_VEC2(name, field) do {\
    ImGui::TableNextColumn();\
    ImGui::Text(name);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat2("##"#field, glm::value_ptr(field));\
    ImGui::TableNextColumn();\
    ImGui::DragFloat2("##"#field"_var", glm::value_ptr(field ## _var));\
} while(0)

#define FLOAT_ROW_VEC3(name, field) do {\
    ImGui::TableNextColumn();\
    ImGui::Text(name);\
    ImGui::TableNextColumn();\
    ImGui::DragFloat3("##"#field, glm::value_ptr(field));\
    ImGui::TableNextColumn();\
    ImGui::DragFloat3("##"#field"_var", glm::value_ptr(field ## _var));\
} while(0)

#define FLOAT_ROW_COLOR(name, field) do {\
    ImGui::TableNextColumn();\
    ImGui::Text(name);\
    ImGui::TableNextColumn();\
    ImGui::ColorEdit3("##"#field, glm::value_ptr(field));\
    ImGui::TableNextColumn();\
    ImGui::ColorEdit3("##"#field"_var", glm::value_ptr(field ## _var));\
} while(0)

struct VariableFloat {
    VariableFloat()
        : value(0.0)
        , variability(0.0)
    {}
    VariableFloat(float value, float variability = 0.0)
        : value(value)
        , variability(variability)
    {}

    float value;
    float variability;
};

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

inline Degrees get_variable_degrees_val(const VariableField<Degrees>& var_degrees) {
    const float curr_variability = random_num_in_range(-var_degrees.variability.val, var_degrees.variability.val);
    return Degrees(var_degrees.base_val.val + curr_variability);
}

inline glm::vec2 get_variable_size_val(const VariableField<glm::vec2>& var_size) {
    const float size_scale = random_num_in_range(0.0, 1.0);
    const glm::vec2 size_var = glm::vec2(
        lerp(-var_size.variability.x, var_size.variability.x, size_scale),
        lerp(-var_size.variability.y, var_size.variability.y, size_scale)
    );

    return var_size.base_val + size_var;
}

inline glm::vec3 get_variable_color_val(const VariableField<glm::vec3>& var_color) {
    const float color_scale = random_num_in_range(0.0, 1.0);
    const glm::vec3 color_var = glm::vec3(
        lerp(-var_color.variability.x, var_color.variability.x, color_scale),
        lerp(-var_color.variability.y, var_color.variability.y, color_scale),
        lerp(-var_color.variability.z, var_color.variability.z, color_scale)
    );

    return var_color.base_val + color_var;
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

    ColoredQuad get_quad(double curr_time, Renderer* renderer, BufferID global_data_buffer, TextureSource texture) const {
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

        return ColoredQuad(
            renderer,
            Rect2D(curr_pos, curr_size),
            texture,
            curr_color,
            global_data_buffer
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

// TODO: for now these live forever, will need a Emmiter that can die soon
struct ParticleEmitter {
    ParticleEmitter(
        double start_time,
        glm::vec2 pos,
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
        , pos(pos)
        , emission_delay(emission_delay)
        , emission_angle(emission_angle)
        , particle_lifetime(particle_lifetime)
        , particle_vel(particle_vel)
        , size(size)
        , color(color)
        , texture(texture)
        , selected_texture(static_cast<int>(texture))
    {}

    void update_and_create_renderables(Renderable& root_renderable, double curr_time, Renderer* renderer, BufferID global_data_buffer) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), 
            [curr_time](Particle& p) {
                return p.anim.is_done_at(curr_time);
            }),
            particles.end()
        );

        const float c_emission_delay = get_variable_float_val(emission_delay);
        if (curr_time - last_updated > c_emission_delay) {
            const float     c_particle_duration = get_variable_float_val(particle_lifetime);
            const Radians   c_particle_angle = get_variable_degrees_val(emission_angle).to_radians();
            const glm::vec2 c_particle_dir = glm::vec2(cos(c_particle_angle.val), sin(c_particle_angle.val));
            const float     c_velocity = get_variable_float_val(particle_vel);
            const glm::vec2 c_start_size = get_variable_size_val(size.start);
            const glm::vec2 c_end_size = get_variable_size_val(size.end);
            const glm::vec3 c_start_color = get_variable_color_val(color.start);
            const glm::vec3 c_end_color = get_variable_color_val(color.end);

            particles.push_back(Particle(curr_time, c_particle_duration, pos, c_particle_dir, c_velocity, c_start_size, c_end_size, c_start_color, c_end_color));
            last_updated = curr_time;
        }

        for (const Particle& particle : particles) {
            // NOTE: All particles use alpha-blending by default
            root_renderable.push_child(Renderable(particle.get_quad(curr_time, renderer, global_data_buffer, texture), true));
        }
    }

    void imgui_node() {
        if (ImGui::BeginTable("Values", 3)) {

            ImGui::TableSetupColumn("Field");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Variability");
            ImGui::TableHeadersRow();

            /*
            FLOAT_ROW("Emission Delay", emission_delay);
            FLOAT_ROW_ACCESSOR("Emission Angle", emission_angle, val);
            FLOAT_ROW("Particle Lifetime", particle_lifetime);
            FLOAT_ROW("Particle Velocity", particle_vel);
            FLOAT_ROW_VEC2("Start Size", size.start);
            FLOAT_ROW_VEC2("End Size", size.end);
            FLOAT_ROW_COLOR("Start Color", color.start);
            FLOAT_ROW_COLOR("End Color", color.end);
            */

            ImGui::EndTable();
        }

        const char**  textures = texture_names();
        bool check =  ImGui::Combo("Texture", &selected_texture, textures, texture_count());
        if (check) {
            texture = static_cast<TextureSource>(selected_texture);
        }
    }

    double start_time;
    double last_updated;

    glm::vec2 pos;

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
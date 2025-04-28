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

#define DEFINE_VARIABLE_FIELD(type, name) type name, name ## _var

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
        float emission_delay,
        Degrees emission_angle,
        float particle_lifetime,
        float particle_vel,
        glm::vec2 start_size,
        glm::vec2 end_size,
        glm::vec3 start_color,
        glm::vec3 end_color,
        TextureSource texture,
        float emission_delay_var = 0.f,
        Degrees emission_angle_var = Degrees(0.f),
        float particle_lifetime_var = 0.f,
        float particle_vel_var = 0.f,
        glm::vec2 start_size_var = glm::vec2(0.0),
        glm::vec2 end_size_var = glm::vec2(0.0),
        glm::vec3 start_color_var = glm::vec3(0.0),
        glm::vec3 end_color_var = glm::vec3(0.0)
    )
        : start_time(start_time)
        , last_updated(start_time)
        , pos(pos)
        , emission_delay(emission_delay)
        , emission_delay_var(emission_delay_var)
        , emission_angle(emission_angle)
        , emission_angle_var(emission_angle_var)
        , particle_lifetime(particle_lifetime)
        , particle_lifetime_var(particle_lifetime_var)
        , particle_vel(particle_vel)
        , particle_vel_var(particle_vel_var)
        , start_size(start_size)
        , start_size_var(start_size_var)
        , end_size(end_size)
        , end_size_var(end_size_var)
        , start_color(start_color)
        , start_color_var(start_color_var)
        , end_color(end_color)
        , end_color_var(end_color_var)
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

        const float curr_emmision_delay_var = 0.0;
        if (curr_time - last_updated > emission_delay + curr_emmision_delay_var) {
            const float curr_particle_lifetime_var = random_num_in_range(-particle_lifetime_var, particle_lifetime_var);
            float particle_duration = particle_lifetime + curr_particle_lifetime_var;

            const Degrees curr_emission_angle_var = Degrees(
                random_num_in_range(-emission_angle_var.val, emission_angle_var.val)
            );
            const Degrees particle_angle = emission_angle + curr_emission_angle_var;
            const Radians particle_angle_r = particle_angle.to_radians();
            const glm::vec2 particle_dir = glm::vec2(cos(particle_angle_r.val), sin(particle_angle_r.val));

            const float curr_particle_vel_var = random_num_in_range(-particle_vel_var, particle_vel_var);
            const float vel = particle_vel + curr_particle_vel_var;

            const float start_size_scale = random_num_in_range(0.0, 1.0);
            const glm::vec2 curr_start_size_var = glm::vec2(
                lerp(-start_size_var.x, start_size_var.x, start_size_scale),
                lerp(-start_size_var.y, start_size_var.y, start_size_scale)
            );
            const glm::vec2 c_start_size = start_size + curr_start_size_var;

            const float end_size_scale = random_num_in_range(0.0, 1.0);
            const glm::vec2 curr_end_size_var = glm::vec2(
                lerp(-end_size_var.x, end_size_var.x, end_size_scale),
                lerp(-end_size_var.y, end_size_var.y, end_size_scale)
            );
            const glm::vec2 c_end_size = end_size + curr_end_size_var;

            const float start_color_scale = random_num_in_range(0.0, 1.0);
            const glm::vec3 curr_start_color_var = glm::vec3(
                lerp(-start_color_var.r, start_color_var.r, start_color_scale),
                lerp(-start_color_var.g, start_color_var.g, start_color_scale),
                lerp(-start_color_var.b, start_color_var.b, start_color_scale)
            );
            const glm::vec3 s_color = start_color + start_color_var;

            const float end_color_scale = random_num_in_range(0.0, 1.0);
            const glm::vec3 curr_end_color_var = glm::vec3(
                lerp(-end_color_var.r, end_color_var.r, end_color_scale),
                lerp(-end_color_var.g, end_color_var.g, end_color_scale),
                lerp(-end_color_var.b, end_color_var.b, end_color_scale)
            );
            const glm::vec3 e_color = end_color + end_color_var;

            particles.push_back(Particle(curr_time, particle_duration, pos, particle_dir, vel, c_start_size, c_end_size, s_color, e_color));
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

            FLOAT_ROW("Emission Delay", emission_delay);
            FLOAT_ROW_ACCESSOR("Emission Angle", emission_angle, val);
            FLOAT_ROW("Particle Lifetime", particle_lifetime);
            FLOAT_ROW("Particle Velocity", particle_vel);
            FLOAT_ROW_VEC2("Start Size", start_size);
            FLOAT_ROW_VEC2("End Size", end_size);
            FLOAT_ROW_COLOR("Start Color", start_color);
            FLOAT_ROW_COLOR("End Color", end_color);

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

    DEFINE_VARIABLE_FIELD(float, emission_delay);
    DEFINE_VARIABLE_FIELD(Degrees, emission_angle);
    DEFINE_VARIABLE_FIELD(float, particle_lifetime);
    DEFINE_VARIABLE_FIELD(float, particle_vel);
    DEFINE_VARIABLE_FIELD(glm::vec2, start_size);
    DEFINE_VARIABLE_FIELD(glm::vec2, end_size);
    DEFINE_VARIABLE_FIELD(glm::vec3, start_color);
    DEFINE_VARIABLE_FIELD(glm::vec3, end_color);

    TextureSource texture;
    int selected_texture;
    std::vector<Particle> particles;
};
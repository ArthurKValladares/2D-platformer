#pragma once

#include <vector>

#include <glm/vec2.hpp>

#include "renderer/renderer.h"
#include "renderables/includes.h"

#include "animatable.h"
#include "assets.h"

#include <iostream>

// TODO: for now all particles scale down in size to 0 until they die, that needs to be variable later
struct Particle {
    Particle(double start_time, double duration, glm::vec2 pos, glm::vec2 dir, float vel, glm::vec2 size) 
        : pos(pos)
        , dir(dir)
        , vel(vel)
        , size(size)
        , anim(AnimatableFloat(0.0, 1.0, duration, start_time))
    {}

    Quad get_quad(double curr_time, Renderer* renderer, BufferID global_data_buffer, TextureSource texture) const {
        const double elapsed_time = curr_time - anim.start_time;
        const double anim_at = anim.value_at(curr_time);
        
        const glm::vec2 curr_pos = pos + dir * glm::vec2(vel * elapsed_time);
        const glm::vec2 curr_size = glm::vec2(lerp(size.x, 0.0, anim_at), lerp(size.y, 0.0, anim_at));

        return Quad(
            renderer,
            Rect2D(curr_pos, curr_size),
            texture,
            global_data_buffer
        );
    }

    glm::vec2 pos;
    glm::vec2 dir;
    float vel;
    glm::vec2 size;
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
        glm::vec2 particle_size,
        TextureSource texture,
        float emission_delay_var = 0.f,
        Degrees emission_angle_var = Degrees(0.f),
        float particle_lifetime_var = 0.f,
        float particle_vel_var = 0.f,
        glm::vec2 particle_size_var = glm::vec2(0.0)
    )
        : start_time(start_time)
        , last_updated(start_time)
        , pos(pos)
        , emission_delay(emission_delay)
        , emmision_delay_var(emmision_delay_var)
        , emission_angle(emission_angle)
        , emission_angle_var(emission_angle_var)
        , particle_lifetime(particle_lifetime)
        , particle_lifetime_var(particle_lifetime_var)
        , particle_vel(particle_vel)
        , particle_vel_var(particle_vel_var)
        , particle_size(particle_size)
        , particle_size_var(particle_size_var)
        , texture(texture)
    {}

    void update_and_create_renderables(Renderable& root_renderable, double curr_time, Renderer* renderer, BufferID global_data_buffer) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), 
            [curr_time](Particle& p) {
                return p.anim.is_done_at(curr_time);
            }),
            particles.end()
        );

        // TODO: All vars are 0 for now, will hook up correctly later
        const float curr_emmision_delay_var = 0.0;
        if (curr_time - last_updated > emission_delay + curr_emmision_delay_var) {
            const float curr_particle_lifetime_var = 0.0;
            float particle_duration = particle_lifetime + curr_particle_lifetime_var;

            const Degrees curr_emission_angle_var = Degrees(0.0);
            const Degrees particle_angle = emission_angle + curr_emission_angle_var;
            const Radians particle_angle_r = particle_angle.to_radians();
            const glm::vec2 particle_dir = glm::vec2(cos(particle_angle_r.val), sin(particle_angle_r.val));

            const float curr_particle_vel_var = 0.0;
            const float vel = particle_vel + curr_particle_vel_var;

            const glm::vec2 curr_particle_size_var = glm::vec2(0.0);
            const glm::vec2 size = particle_size + curr_particle_size_var;

            particles.push_back(Particle(curr_time, particle_duration, pos, particle_dir, vel, size));
            last_updated = curr_time;
        }

        for (const Particle& particle : particles) {
            root_renderable.push_child(particle.get_quad(curr_time, renderer, global_data_buffer, texture));
        }
    }

    double start_time;
    double last_updated;
    glm::vec2 pos;

    float emission_delay;
    float emmision_delay_var;

    Degrees emission_angle;
    Degrees emission_angle_var;

    float particle_lifetime;
    float particle_lifetime_var;

    float particle_vel;
    float particle_vel_var;
    
    glm::vec2 particle_size;
    glm::vec2 particle_size_var;

    TextureSource texture;
    std::vector<Particle> particles;
};
#pragma once

#include "particles.h"
#include "json_serialization.h"
#include "load_save.h"

struct ParticleLoader {
    ParticleLoader() {}

    void save(ImguiLog* logger, const ParticleEmitter& emitter) {
        load_save.save(logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            serialize_float(root, "lifetime", emitter.lifetime.val);
            serialize_vec2(root, "center", emitter.center);
            serialize_vec2(root, "start_offset", emitter.start_offset.base_val);
            serialize_vec2(root, "start_offset_var", emitter.start_offset.variability);
            serialize_float(root, "emission_delay", emitter.emission_delay.base_val);
            serialize_float(root, "emission_delay_var", emitter.emission_delay.variability);
            serialize_float(root, "emission_angle", emitter.emission_angle.base_val.val);
            serialize_float(root, "emission_angle_var", emitter.emission_angle.variability.val);
            serialize_float(root, "particle_lifetime", emitter.particle_lifetime.base_val);
            serialize_float(root, "particle_lifetime_var", emitter.particle_lifetime.variability);
            serialize_float(root, "particle_vel", emitter.particle_vel.base_val);
            serialize_float(root, "particle_vel_var", emitter.particle_vel.variability);
            serialize_vec2(root, "start_size", emitter.size.start.base_val);
            serialize_vec2(root, "start_size_var", emitter.size.start.variability);
            serialize_vec2(root, "end_size", emitter.size.end.base_val);
            serialize_vec2(root, "end_size_var", emitter.size.end.variability);
            serialize_vec3(root, "start_color", emitter.color.start.base_val);
            serialize_vec3(root, "start_color_var", emitter.color.start.variability);
            serialize_vec3(root, "end_color", emitter.color.end.base_val);
            serialize_vec3(root, "end_color_var", emitter.color.end.variability);
            serialize_uint32(root, "texture", static_cast<uint32_t>(emitter.texture));
        });
    }

    void load_from(ImguiLog* logger, ParticleEmitter& emitter, const char* file, double total_elapsed_seconds) {
        load_save.load_from(logger, PARTICLES_DIR, PARTICLES_EXTENSION, file, [&](nlohmann::json& root) {
            emitter = ParticleEmitter(
                total_elapsed_seconds,
                EmitterLifetime(get_serialized_float(root, "lifetime")),
                get_serialized_vec2(root, "center"),
                VariableField<glm::vec2>(
                    get_serialized_vec2(root, "start_offset"),
                    get_serialized_vec2(root, "start_offset_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "emission_delay"),
                    get_serialized_float(root, "emission_delay_var")
                ),
                VariableField<Degrees>(
                    Degrees(get_serialized_float(root, "emission_angle")),
                    Degrees(get_serialized_float(root, "emission_angle_var"))
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_lifetime"),
                    get_serialized_float(root, "particle_lifetime_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_vel"),
                    get_serialized_float(root, "particle_vel_var")
                ),
                InterpolatableField<VariableField<glm::vec2>>(
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "start_size"),
                        get_serialized_vec2(root, "start_size_var")
                    ),
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "end_size"),
                        get_serialized_vec2(root, "end_size_var")
                    )
                ),
                InterpolatableField<VariableField<glm::vec3>>(
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "start_color"),
                        get_serialized_vec3(root, "start_color_var")
                    ),
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "end_color"),
                        get_serialized_vec3(root, "end_color_var")
                    )
                ),
                static_cast<TextureSource>(get_serialized_uint32(root, "texture"))
            );
        });
    }
    void load(ImguiLog* logger, ParticleEmitter& emitter, double total_elapsed_seconds) {
        // TODO: THis is again very bad, big refactor coming
        load_save.load(logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            emitter = ParticleEmitter(
                total_elapsed_seconds,
                EmitterLifetime(get_serialized_float(root, "lifetime")),
                get_serialized_vec2(root, "center"),
                VariableField<glm::vec2>(
                    get_serialized_vec2(root, "start_offset"),
                    get_serialized_vec2(root, "start_offset_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "emission_delay"),
                    get_serialized_float(root, "emission_delay_var")
                ),
                VariableField<Degrees>(
                    Degrees(get_serialized_float(root, "emission_angle")),
                    Degrees(get_serialized_float(root, "emission_angle_var"))
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_lifetime"),
                    get_serialized_float(root, "particle_lifetime_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_vel"),
                    get_serialized_float(root, "particle_vel_var")
                ),
                InterpolatableField<VariableField<glm::vec2>>(
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "start_size"),
                        get_serialized_vec2(root, "start_size_var")
                    ),
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "end_size"),
                        get_serialized_vec2(root, "end_size_var")
                    )
                ),
                InterpolatableField<VariableField<glm::vec3>>(
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "start_color"),
                        get_serialized_vec3(root, "start_color_var")
                    ),
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "end_color"),
                        get_serialized_vec3(root, "end_color_var")
                    )
                ),
                static_cast<TextureSource>(get_serialized_uint32(root, "texture"))
            );
        });
    }

    LoadSave load_save;
};
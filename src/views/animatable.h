#pragma once

#include <vector>
#include <cmath>

#include "../assets.h"

inline double lerp(double a, double b, double f)
{
    return a * (1.0 - f) + (b * f);
}

// TODO: For now this just keeps repeating infinitely,
// Later I need animations that can end.
struct AnimatableFloat {
    AnimatableFloat() {}
    AnimatableFloat(
        double start_val, double end_val,
        double duration,
        double start_time
    )
        : start_val(start_val)
        , end_val(end_val)
        , duration(duration)
        , start_time(start_time)
    {}

    double value_at(double curr_time) {
        if (duration == 0.0) {
            assert(false);
            return start_val;
        }
        const double elapsed = curr_time - start_time;
        const double remainder = fmod(elapsed, duration);
        const double progress = remainder / duration;
        return lerp(start_val, end_val, remainder);
    }

    double start_val, end_val;
    double duration;
    // NOTE: This is start_time in elapsed seconds since the start of the app.
    // Probably need something better later
    double start_time;
};

struct SpriteAnimation {
    SpriteAnimation()
    {}
    SpriteAnimation(double duration, double start_time, std::vector<TextureSource> textures)
        : animatable(AnimatableFloat(0.0, 1.0, duration, start_time))
        , textures(std::move(textures))
    {}

    TextureSource texture_at(double curr_time) {
        const double val_at = animatable.value_at(curr_time);
        const uint32_t texture_idx = std::roundf(val_at * textures.size());
        return textures[texture_idx];
    }

    AnimatableFloat animatable;
    std::vector<TextureSource> textures;
};
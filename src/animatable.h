#pragma once

#include <vector>
#include <cmath>

#include "assets.h"
#include "util.h"

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

    bool is_done_at(double curr_time) const {
        const double elapsed = curr_time - start_time;
        const double progress = elapsed / duration;
        return progress >= 1.0;
    }

    double value_at(double curr_time) const {
        if (duration == 0.0) {
            assert(false);
            return start_val;
        }
        const double elapsed = curr_time - start_time;
        const double progress = elapsed / duration;
        if (progress >= 1.0) {
            return end_val;
        }
        return lerp(start_val, end_val, progress);
    }

    double start_val, end_val;
    double duration;
    // NOTE: This is start_time in elapsed seconds since the start of the app.
    // Probably need something better later
    double start_time;
};

struct RepeatableAnimatableFloat {
    RepeatableAnimatableFloat() {}
    RepeatableAnimatableFloat(
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
        return lerp(start_val, end_val, progress);
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
        : animatable(RepeatableAnimatableFloat(0.0, 1.0, duration, start_time))
        , textures(std::move(textures))
    {}

    TextureSource texture_at(double curr_time) {
        const double val_at = animatable.value_at(curr_time);
        const uint32_t texture_idx = std::floor(val_at * textures.size());
        return textures[texture_idx];
    }

    RepeatableAnimatableFloat animatable;
    std::vector<TextureSource> textures;
};
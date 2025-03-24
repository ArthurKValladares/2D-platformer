#pragma once

#pragma once


#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct OrthographicCamera {
    OrthographicCamera(float size_x, float size_y)
        : size_x(size_x)
        , size_y(size_y)
    {}

    glm::mat4 get_proj_matrix();

    float size_x;
    float size_y;
};
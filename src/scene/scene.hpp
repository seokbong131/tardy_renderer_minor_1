#pragma once

#include <vector>

#include "graphics_mathematics/matrix.hpp"
#include "obj_loader.h"
#include "scene/light.hpp"
#include "scene/material.hpp"

struct SceneObject
{
    // non-owning pointer
    Mesh* mesh = nullptr;

    // by value
    mat4     model_mat = identity<4>();
    Material material;
};

// flat collection
struct Scene
{
    std::vector<SceneObject> objects;
    std::vector<Light>       lights;
};

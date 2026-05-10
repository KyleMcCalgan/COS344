#ifndef PALETTE_HPP
#define PALETTE_HPP

#include "Input.hpp"

struct Color3f
{
    float r;
    float g;
    float b;
};

static const Color3f FLOOR_COLORS[10] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.5f, 0.0f},
    {0.5f, 0.0f, 0.5f}
};

static const Color3f BALL_COLORS[10] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.5f, 0.0f},
    {0.5f, 0.0f, 0.5f}
};

static const Color3f LIGHT_COLORS[9] = {
    {1.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.5f, 0.0f},
    {0.5f, 0.0f, 0.5f}
};

inline Color3f getFloorColor(const InputState &input)
{
    return FLOOR_COLORS[input.floorColorIndex];
}

inline Color3f getBallColor(const InputState &input)
{
    return BALL_COLORS[input.ballColorIndex];
}

inline Color3f getLightColor(const InputState &input)
{
    return LIGHT_COLORS[input.lightColorIndex];
}

#endif

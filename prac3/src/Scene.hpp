#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <GL/glew.h>
#include "Mesh.hpp"
#include "math3d.hpp"
#include "Input.hpp"

struct MeshEntry
{
    Mesh mesh;
    bool isRotor = false; // true → gets rotor MVP, false → gets scene MVP
};

class Scene
{
public:
    Scene();
    ~Scene();

    // Build all geometry and upload to GPU.
    void build();

    // Draw the whole scene given current input state and delta time.
    void draw(GLuint shaderProgram, const InputState& input, float dt);

private:
    std::vector<MeshEntry> m_entries;
    float m_rotorAngle = 0.0f;

    // Hub position (world space) — the pivot point for rotor spin.
    static constexpr float HUB_X = 0.0f;
    static constexpr float HUB_Y = 2.5f;
    static constexpr float HUB_Z = 0.80f;
};

#endif // SCENE_HPP

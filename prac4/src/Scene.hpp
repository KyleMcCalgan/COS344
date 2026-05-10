#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/glew.h>

#include "Mesh.h"
#include "Input.hpp"
#include "Textures.hpp"

class Scene
{
public:
    Scene();
    ~Scene();

    void build(const InputState &input);
    void syncGeometry(InputState &input);
    void draw(GLuint shaderProgram, const InputState &input, int framebufferWidth,
              int framebufferHeight) const;

private:
    Mesh m_sphereMesh;
    Mesh m_planeMesh;
    Mesh m_lightMarkerMesh;
    TextureSet m_textures;

    void rebuildSphere(int subdivisionLevel);
    void rebuildPlane(int resolution);
    void rebuildLightMarker();
};

#endif

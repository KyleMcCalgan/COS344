#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <GL/glew.h>

class TextureSet
{
public:
    TextureSet();
    ~TextureSet();

    void loadAll();
    void bindAll(GLuint shaderProgram) const;

private:
    GLuint m_colourTextureId;
    GLuint m_displacementTextureId;
    GLuint m_alphaTextureId;

    GLuint loadBMPTexture(const char *path) const;
    void release();
};

#endif

#ifndef HUD_HPP
#define HUD_HPP

#include <GL/glew.h>

#include "Input.hpp"
#include "linefont.h"

class HUD
{
public:
    HUD();
    ~HUD();

    void init();
    void draw(const InputState &input, int winW, int winH);

private:
    GLuint m_shader;
    LineFont m_font;
    GLuint m_colorLoc;
    GLuint m_modelLoc;
    GLuint m_projectionLoc;
};

#endif

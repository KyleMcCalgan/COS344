#ifndef HUD_HPP
#define HUD_HPP

#include <GL/glew.h>
#include <string>
#include "linefont.h"
#include "Input.hpp"

// HUD — draws 2D text overlay using the linefont GL_LINES renderer.
// Uses a separate shader program (HUDVertex.glsl / HUDFragment.glsl) so it
// doesn't interfere with the 3D MVP shader.
class HUD
{
public:
    HUD();
    ~HUD();

    // Load the HUD shader pair and initialise the LineFont renderer.
    // Call once after the OpenGL context is created.
    void init();

    // Draw the HUD overlay for the current frame.
    // winW / winH: current window size in pixels (for projection).
    void draw(const InputState& input, int winW, int winH);

private:
    GLuint    m_shader   = 0;
    LineFont  m_font;
    GLuint    m_colLoc   = 0;
    GLuint    m_modLoc   = 0;
    GLuint    m_projLoc  = 0;
};

#endif // HUD_HPP

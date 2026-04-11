#include "HUD.hpp"
#include "../shader.hpp"
#include <sstream>
#include <iomanip>

HUD::HUD()  = default;
HUD::~HUD()
{
    if (m_shader) glDeleteProgram(m_shader);
}

void HUD::init()
{
    m_shader  = LoadShaders("HUDVertex.glsl", "HUDFragment.glsl");
    m_colLoc  = glGetUniformLocation(m_shader, "colour");
    m_modLoc  = glGetUniformLocation(m_shader, "model");
    m_projLoc = glGetUniformLocation(m_shader, "projection");
    m_font.init(m_shader, m_colLoc, m_modLoc, m_projLoc);
}

void HUD::draw(const InputState& input, int winW, int winH)
{
    // Orthographic projection: maps pixel coords → NDC.
    // The matrix maps (0..winW, 0..winH) → (-1..1, -1..1).
    float sx = 2.0f / winW;
    float sy = 2.0f / winH;
    float proj[9] = {
        sx,  0,  -1.0f,
        0,  sy,  -1.0f,
        0,   0,   1.0f
    };

    // Disable depth test while drawing the 2D overlay so it always appears on top.
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_shader);

    float charW = 12.0f;  // pixels per character cell
    float charH = 18.0f;

    // Line 1: wireframe mode indicator
    std::string modeStr = input.wireframe ? "MODE: WIREFRAME" : "MODE: SOLID";
    m_font.drawString(modeStr, 10.0f, winH - charH - 8.0f,
                      charW, charH,
                      1.0f, 1.0f, 0.2f,   // yellow
                      proj);

    // Line 2: rotor speed
    std::ostringstream ss;
    ss << "SPEED: " << std::fixed << std::setprecision(1) << input.rotorSpeed;
    m_font.drawString(ss.str(), 10.0f, winH - 2.0f * charH - 16.0f,
                      charW, charH,
                      0.4f, 1.0f, 0.4f,   // light green
                      proj);

    // Line 3: controls reminder
    m_font.drawString("WASDQE: ROTATE   IJKLOU: MOVE   +/-: SPEED   ENTER: WIREFRAME",
                      10.0f, 10.0f,
                      8.0f, 12.0f,
                      0.8f, 0.8f, 0.8f,   // light grey
                      proj);

    glEnable(GL_DEPTH_TEST);
}

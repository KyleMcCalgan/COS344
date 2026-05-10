#include "HUD.hpp"

#include <iomanip>
#include <sstream>
#include <string>

#include "Palette.hpp"
#include "../shader.hpp"

HUD::HUD()
    : m_shader(0), m_colorLoc(0), m_modelLoc(0), m_projectionLoc(0)
{
}

HUD::~HUD()
{
    if (m_shader != 0)
    {
        glDeleteProgram(m_shader);
    }
}

void HUD::init()
{
    m_shader = LoadShaders("HUDVertex.glsl", "HUDFragment.glsl");
    m_colorLoc = glGetUniformLocation(m_shader, "colour");
    m_modelLoc = glGetUniformLocation(m_shader, "model");
    m_projectionLoc = glGetUniformLocation(m_shader, "projection");
    m_font.init(m_shader, m_colorLoc, m_modelLoc, m_projectionLoc);
}

void HUD::draw(const InputState &input, int winW, int winH)
{
    float sx = 2.0f / static_cast<float>(winW);
    float sy = 2.0f / static_cast<float>(winH);
    float proj[9] = {
        sx, 0.0f, -1.0f,
        0.0f, sy, -1.0f,
        0.0f, 0.0f, 1.0f
    };

    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_shader);

    float leftX = 10.0f;
    float rightX = static_cast<float>(winW) - 280.0f;
    float rowY = static_cast<float>(winH) - 22.0f;
    float rowStep = 16.0f;
    float labelWidth = 11.0f;
    float valueWidth = 8.0f;
    float keyX = 170.0f;

    m_font.drawString("ACTION", leftX, rowY, 9.0f, 13.0f, 1.0f, 0.95f, 0.35f, proj);
    m_font.drawString("KEY", keyX, rowY, 9.0f, 13.0f, 1.0f, 0.95f, 0.35f, proj);

    rowY -= rowStep;
    m_font.drawString("SPHERE DETAIL", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString("I / K", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    rowY -= rowStep;
    m_font.drawString("PLANE DETAIL", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString("O / L", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    rowY -= rowStep;
    m_font.drawString("SELECT COLOUR", leftX, rowY, valueWidth, 12.0f, 0.65f, 0.95f, 0.65f, proj);
    m_font.drawString("1 / 2 / 3", keyX, rowY, valueWidth, 12.0f, 0.65f, 0.95f, 0.65f, proj);

    rowY -= rowStep;
    m_font.drawString("CYCLE COLOUR", leftX, rowY, valueWidth, 12.0f, 0.65f, 0.95f, 0.65f, proj);
    m_font.drawString("[ / ]", keyX, rowY, valueWidth, 12.0f, 0.65f, 0.95f, 0.65f, proj);

    rowY -= rowStep;
    m_font.drawString("BALL ALPHA", leftX, rowY, valueWidth, 12.0f, 0.80f, 0.85f, 0.95f, proj);
    m_font.drawString("+ / -", keyX, rowY, valueWidth, 12.0f, 0.80f, 0.85f, 0.95f, proj);

    rowY -= rowStep;
    m_font.drawString("TEXTURES", leftX, rowY, valueWidth, 12.0f, 0.95f, 0.75f, 0.75f, proj);
    m_font.drawString("B / N / M", keyX, rowY, valueWidth, 12.0f, 0.95f, 0.75f, 0.75f, proj);

    rowY -= rowStep;
    m_font.drawString("ROTATE SCENE", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString("W A S D E Q", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    rowY -= rowStep;
    m_font.drawString("MOVE LIGHT XY", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString("ARROWS", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    rowY -= rowStep;
    m_font.drawString("MOVE LIGHT Z", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString(", / .", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    rowY -= rowStep;
    m_font.drawString("RESET", leftX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);
    m_font.drawString("SPACE", keyX, rowY, valueWidth, 12.0f, 0.90f, 0.90f, 0.90f, proj);

    Color3f floorColor = getFloorColor(input);
    Color3f ballColor = getBallColor(input);
    Color3f lightColor = getLightColor(input);

    float infoY = static_cast<float>(winH) - 22.0f;

    m_font.drawString("STATUS", rightX, infoY, labelWidth, 13.0f, 1.0f, 0.95f, 0.35f, proj);

    infoY -= rowStep;
    std::ostringstream alphaStream;
    alphaStream << std::fixed << std::setprecision(2) << input.ballAlpha;
    m_font.drawString("MODE", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    std::string modeLabel = input.wireframeMode ? "WIREFRAME" : "SOLID";
    m_font.drawString(modeLabel, rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 1.0f, 0.95f, 0.35f, proj);

    infoY -= rowStep;
    m_font.drawString("COLOUR SEL", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    m_font.drawString(getSelectedColorTargetLabel(input), rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 1.0f, 0.95f, 0.35f, proj);

    infoY -= rowStep;
    m_font.drawString("ALPHA", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    m_font.drawString(alphaStream.str(), rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 1.0f, 0.95f, 0.35f, proj);

    infoY -= rowStep;
    m_font.drawString("COLOUR TEX", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    m_font.drawString(input.colourTextureEnabled ? "ON" : "OFF", rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 0.95f, 0.75f, 0.75f, proj);

    infoY -= rowStep;
    m_font.drawString("DISPLACE", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    m_font.drawString(input.displacementTextureEnabled ? "ON" : "OFF", rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 0.95f, 0.75f, 0.75f, proj);

    infoY -= rowStep;
    m_font.drawString("ALPHA TEX", rightX, infoY, valueWidth, 12.0f, 0.95f, 0.95f, 0.95f, proj);
    m_font.drawString(input.alphaTextureEnabled ? "ON" : "OFF", rightX + 120.0f, infoY,
                      valueWidth, 12.0f, 0.95f, 0.75f, 0.75f, proj);

    infoY -= rowStep * 2.0f;
    m_font.drawString("FLOOR", rightX, infoY, 8.0f, 12.0f,
                      floorColor.r, floorColor.g, floorColor.b, proj);
    infoY -= 18.0f;
    m_font.drawString("BALL", rightX, infoY, 8.0f, 12.0f,
                      ballColor.r, ballColor.g, ballColor.b, proj);
    infoY -= 18.0f;
    m_font.drawString("LIGHT", rightX, infoY, 8.0f, 12.0f,
                      lightColor.r, lightColor.g, lightColor.b, proj);

    glEnable(GL_DEPTH_TEST);
}

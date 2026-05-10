#include "Textures.hpp"

#include <fstream>
#include <vector>

namespace
{
unsigned int readLittleEndian32(const unsigned char *data)
{
    return static_cast<unsigned int>(data[0]) |
           (static_cast<unsigned int>(data[1]) << 8) |
           (static_cast<unsigned int>(data[2]) << 16) |
           (static_cast<unsigned int>(data[3]) << 24);
}

unsigned short readLittleEndian16(const unsigned char *data)
{
    return static_cast<unsigned short>(data[0]) |
           (static_cast<unsigned short>(data[1]) << 8);
}
}

TextureSet::TextureSet()
    : m_colourTextureId(0), m_displacementTextureId(0), m_alphaTextureId(0)
{
}

TextureSet::~TextureSet()
{
    release();
}

void TextureSet::loadAll()
{
    release();
    m_colourTextureId = loadBMPTexture("textures/colour/colour.bmp");
    m_displacementTextureId = loadBMPTexture("textures/displacement/displacement.bmp");
    m_alphaTextureId = loadBMPTexture("textures/alpha/alpha.bmp");
}

void TextureSet::bindAll(GLuint shaderProgram) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colourTextureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "colourMap"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_displacementTextureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "displacementMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "alphaMap"), 2);
}

GLuint TextureSet::loadBMPTexture(const char *path) const
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw "Failed to open BMP texture";
    }

    unsigned char header[54];
    file.read(reinterpret_cast<char *>(header), 54);
    if (!file || header[0] != 'B' || header[1] != 'M')
    {
        throw "Invalid BMP texture file";
    }

    unsigned int dataOffset = readLittleEndian32(&header[10]);
    unsigned int width = readLittleEndian32(&header[18]);
    unsigned int height = readLittleEndian32(&header[22]);
    unsigned short bitsPerPixel = readLittleEndian16(&header[28]);
    unsigned int compression = readLittleEndian32(&header[30]);

    if (bitsPerPixel != 24 || compression != 0)
    {
        throw "Only uncompressed 24-bit BMP textures are supported";
    }

    const unsigned int bytesPerPixel = 3;
    const unsigned int rowStride = width * bytesPerPixel;
    const unsigned int paddedRowStride = (rowStride + 3U) & ~3U;
    std::vector<unsigned char> fileData(paddedRowStride * height);

    file.seekg(static_cast<std::streamoff>(dataOffset), std::ios::beg);
    file.read(reinterpret_cast<char *>(&fileData[0]),
              static_cast<std::streamsize>(fileData.size()));
    if (!file)
    {
        throw "Failed to read BMP texture data";
    }

    std::vector<unsigned char> rgbData(rowStride * height);
    for (unsigned int row = 0; row < height; row++)
    {
        const unsigned int sourceRow = height - 1U - row;
        const unsigned char *source = &fileData[sourceRow * paddedRowStride];
        unsigned char *destination = &rgbData[row * rowStride];

        for (unsigned int col = 0; col < width; col++)
        {
            destination[col * 3 + 0] = source[col * 3 + 2];
            destination[col * 3 + 1] = source[col * 3 + 1];
            destination[col * 3 + 2] = source[col * 3 + 0];
        }
    }

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 &rgbData[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureId;
}

void TextureSet::release()
{
    if (m_alphaTextureId != 0)
    {
        glDeleteTextures(1, &m_alphaTextureId);
        m_alphaTextureId = 0;
    }

    if (m_displacementTextureId != 0)
    {
        glDeleteTextures(1, &m_displacementTextureId);
        m_displacementTextureId = 0;
    }

    if (m_colourTextureId != 0)
    {
        glDeleteTextures(1, &m_colourTextureId);
        m_colourTextureId = 0;
    }
}

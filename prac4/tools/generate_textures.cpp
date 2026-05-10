#include <cmath>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace
{
const int TEXTURE_SIZE = 256;
const int DIMPLE_ROWS = 14;
const int DIMPLE_COLUMNS = 16;
const float DIMPLE_RADIUS_SCALE = 0.34f;

struct Pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

void ensureTexturesDirectory()
{
    mkdir("textures", 0755);
    mkdir("textures/colour", 0755);
    mkdir("textures/displacement", 0755);
    mkdir("textures/alpha", 0755);
}

float clamp01(float value)
{
    if (value < 0.0f)
    {
        return 0.0f;
    }

    if (value > 1.0f)
    {
        return 1.0f;
    }

    return value;
}

float smoothstep(float edge0, float edge1, float x)
{
    float t = clamp01((x - edge0) / (edge1 - edge0));
    return t * t * (3.0f - 2.0f * t);
}

float wrappedDistanceX(float a, float b)
{
    float dx = std::fabs(a - b);
    return (dx > 0.5f) ? (1.0f - dx) : dx;
}

float computeDimpleMask(float u, float v)
{
    const float cellWidth = 1.0f / static_cast<float>(DIMPLE_COLUMNS);
    const float cellHeight = 1.0f / static_cast<float>(DIMPLE_ROWS);
    const float radius = DIMPLE_RADIUS_SCALE * ((cellWidth < cellHeight) ? cellWidth : cellHeight);
    const float innerRadius = radius * 0.70f;

    float strongestMask = 0.0f;

    for (int row = 0; row < DIMPLE_ROWS; row++)
    {
        float centerV = (static_cast<float>(row) + 0.5f) * cellHeight;
        float rowOffset = (row % 2 == 0) ? 0.0f : (0.5f * cellWidth);

        for (int col = 0; col < DIMPLE_COLUMNS; col++)
        {
            float centerU = (static_cast<float>(col) + 0.5f) * cellWidth + rowOffset;
            while (centerU >= 1.0f)
            {
                centerU -= 1.0f;
            }

            float dx = wrappedDistanceX(u, centerU);
            float dy = std::fabs(v - centerV);
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < radius)
            {
                float mask = 1.0f - smoothstep(innerRadius, radius, distance);
                if (mask > strongestMask)
                {
                    strongestMask = mask;
                }
            }
        }
    }

    return strongestMask;
}

void writeBMP(const std::string &path, const std::vector<Pixel> &pixels, int width, int height)
{
    const int bytesPerPixel = 3;
    const int rowStride = width * bytesPerPixel;
    const int paddedRowStride = (rowStride + 3) & ~3;
    const int imageSize = paddedRowStride * height;
    const int fileSize = 54 + imageSize;

    unsigned char header[54] = {0};
    header[0] = 'B';
    header[1] = 'M';
    header[2] = static_cast<unsigned char>(fileSize & 0xFF);
    header[3] = static_cast<unsigned char>((fileSize >> 8) & 0xFF);
    header[4] = static_cast<unsigned char>((fileSize >> 16) & 0xFF);
    header[5] = static_cast<unsigned char>((fileSize >> 24) & 0xFF);
    header[10] = 54;
    header[14] = 40;
    header[18] = static_cast<unsigned char>(width & 0xFF);
    header[19] = static_cast<unsigned char>((width >> 8) & 0xFF);
    header[20] = static_cast<unsigned char>((width >> 16) & 0xFF);
    header[21] = static_cast<unsigned char>((width >> 24) & 0xFF);
    header[22] = static_cast<unsigned char>(height & 0xFF);
    header[23] = static_cast<unsigned char>((height >> 8) & 0xFF);
    header[24] = static_cast<unsigned char>((height >> 16) & 0xFF);
    header[25] = static_cast<unsigned char>((height >> 24) & 0xFF);
    header[26] = 1;
    header[28] = 24;
    header[34] = static_cast<unsigned char>(imageSize & 0xFF);
    header[35] = static_cast<unsigned char>((imageSize >> 8) & 0xFF);
    header[36] = static_cast<unsigned char>((imageSize >> 16) & 0xFF);
    header[37] = static_cast<unsigned char>((imageSize >> 24) & 0xFF);

    std::ofstream output(path.c_str(), std::ios::binary);
    output.write(reinterpret_cast<const char *>(header), 54);

    std::vector<unsigned char> padding(paddedRowStride - rowStride, 0);
    for (int row = height - 1; row >= 0; row--)
    {
        for (int col = 0; col < width; col++)
        {
            const Pixel &pixel = pixels[row * width + col];
            unsigned char bgr[3] = {pixel.b, pixel.g, pixel.r};
            output.write(reinterpret_cast<const char *>(bgr), 3);
        }

        if (!padding.empty())
        {
            output.write(reinterpret_cast<const char *>(&padding[0]),
                         static_cast<std::streamsize>(padding.size()));
        }
    }
}

unsigned char interpolateByte(float mask, unsigned char outsideValue, unsigned char insideValue)
{
    float value = (1.0f - mask) * static_cast<float>(outsideValue) +
                  mask * static_cast<float>(insideValue);
    return static_cast<unsigned char>(value);
}
}

int main()
{
    ensureTexturesDirectory();

    std::vector<Pixel> colourPixels(TEXTURE_SIZE * TEXTURE_SIZE);
    std::vector<Pixel> displacementPixels(TEXTURE_SIZE * TEXTURE_SIZE);
    std::vector<Pixel> alphaPixels(TEXTURE_SIZE * TEXTURE_SIZE);

    for (int y = 0; y < TEXTURE_SIZE; y++)
    {
        for (int x = 0; x < TEXTURE_SIZE; x++)
        {
            float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(TEXTURE_SIZE);
            float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(TEXTURE_SIZE);
            float mask = computeDimpleMask(u, v);

            Pixel colour;
            colour.r = interpolateByte(mask, 220, 80);
            colour.g = interpolateByte(mask, 220, 80);
            colour.b = interpolateByte(mask, 220, 80);

            Pixel displacement;
            unsigned char displacementValue = interpolateByte(mask, 255, 0);
            displacement.r = displacementValue;
            displacement.g = displacementValue;
            displacement.b = displacementValue;

            Pixel alpha;
            unsigned char alphaValue = interpolateByte(mask, 255, 0);
            alpha.r = alphaValue;
            alpha.g = alphaValue;
            alpha.b = alphaValue;

            colourPixels[y * TEXTURE_SIZE + x] = colour;
            displacementPixels[y * TEXTURE_SIZE + x] = displacement;
            alphaPixels[y * TEXTURE_SIZE + x] = alpha;
        }
    }

    writeBMP("textures/colour/golf_colour.bmp", colourPixels, TEXTURE_SIZE, TEXTURE_SIZE);
    writeBMP("textures/displacement/golf_displacement.bmp", displacementPixels, TEXTURE_SIZE, TEXTURE_SIZE);
    writeBMP("textures/alpha/golf_alpha.bmp", alphaPixels, TEXTURE_SIZE, TEXTURE_SIZE);
    return 0;
}

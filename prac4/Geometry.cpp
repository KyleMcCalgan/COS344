#include "Geometry.h"

#include <cmath>

namespace
{
const int FLOATS_PER_VERTEX = 8;
const float PI = 3.14159265359f;

float clampPositive(float value)
{
    return (value < 0.0f) ? 0.0f : value;
}
}

MeshData generateSphereMesh(int subdivisionLevel, float radius)
{
    const int stacks = subdivisionLevel;
    const int slices = subdivisionLevel;
    const int vertexCount = (stacks + 1) * (slices + 1);
    const int floatCount = vertexCount * FLOATS_PER_VERTEX;
    const int indexCount = 6 * slices * (stacks - 1);

    float *vertexData = new float[floatCount];
    unsigned int *indexData = new unsigned int[indexCount];

    int floatOffset = 0;
    for (int stack = 0; stack <= stacks; stack++)
    {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float phi = PI * v;
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (int slice = 0; slice <= slices; slice++)
        {
            const float u = static_cast<float>(slice) / static_cast<float>(slices);
            const float theta = 2.0f * PI * u;
            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            const float x = sinPhi * cosTheta;
            const float y = cosPhi;
            const float z = sinPhi * sinTheta;

            vertexData[floatOffset++] = radius * x;
            vertexData[floatOffset++] = radius * y;
            vertexData[floatOffset++] = radius * z;

            vertexData[floatOffset++] = x;
            vertexData[floatOffset++] = y;
            vertexData[floatOffset++] = z;

            vertexData[floatOffset++] = u;
            vertexData[floatOffset++] = v;
        }
    }

    const int rowLength = slices + 1;
    int indexOffset = 0;
    for (int stack = 0; stack < stacks; stack++)
    {
        for (int slice = 0; slice < slices; slice++)
        {
            const unsigned int topLeft = static_cast<unsigned int>(stack * rowLength + slice);
            const unsigned int topRight = topLeft + 1U;
            const unsigned int bottomLeft = topLeft + static_cast<unsigned int>(rowLength);
            const unsigned int bottomRight = bottomLeft + 1U;

            if (stack == 0)
            {
                indexData[indexOffset++] = topLeft;
                indexData[indexOffset++] = bottomLeft;
                indexData[indexOffset++] = bottomRight;
                continue;
            }

            if (stack == stacks - 1)
            {
                indexData[indexOffset++] = topLeft;
                indexData[indexOffset++] = bottomLeft;
                indexData[indexOffset++] = topRight;
                continue;
            }

            indexData[indexOffset++] = topLeft;
            indexData[indexOffset++] = bottomLeft;
            indexData[indexOffset++] = topRight;

            indexData[indexOffset++] = topRight;
            indexData[indexOffset++] = bottomLeft;
            indexData[indexOffset++] = bottomRight;
        }
    }

    MeshData meshData;
    meshData.vertexData = vertexData;
    meshData.indexData = indexData;
    meshData.floatCount = floatCount;
    meshData.indexCount = indexCount;
    return meshData;
}

MeshData generatePlaneMesh(int resolution, float yPosition, float extent)
{
    const int verticesPerSide = resolution + 1;
    const int vertexCount = verticesPerSide * verticesPerSide;
    const int floatCount = vertexCount * FLOATS_PER_VERTEX;
    const int indexCount = resolution * resolution * 6;

    float *vertexData = new float[floatCount];
    unsigned int *indexData = new unsigned int[indexCount];

    int floatOffset = 0;
    for (int row = 0; row < verticesPerSide; row++)
    {
        const float v = static_cast<float>(row) / static_cast<float>(resolution);
        const float z = -extent + (2.0f * extent * v);

        for (int col = 0; col < verticesPerSide; col++)
        {
            const float u = static_cast<float>(col) / static_cast<float>(resolution);
            const float x = -extent + (2.0f * extent * u);

            vertexData[floatOffset++] = x;
            vertexData[floatOffset++] = yPosition;
            vertexData[floatOffset++] = z;

            vertexData[floatOffset++] = 0.0f;
            vertexData[floatOffset++] = 1.0f;
            vertexData[floatOffset++] = 0.0f;

            vertexData[floatOffset++] = u;
            vertexData[floatOffset++] = v;
        }
    }

    int indexOffset = 0;
    for (int row = 0; row < resolution; row++)
    {
        for (int col = 0; col < resolution; col++)
        {
            const unsigned int topLeft = static_cast<unsigned int>(row * verticesPerSide + col);
            const unsigned int topRight = topLeft + 1U;
            const unsigned int bottomLeft = topLeft + static_cast<unsigned int>(verticesPerSide);
            const unsigned int bottomRight = bottomLeft + 1U;

            indexData[indexOffset++] = topLeft;
            indexData[indexOffset++] = bottomLeft;
            indexData[indexOffset++] = topRight;

            indexData[indexOffset++] = topRight;
            indexData[indexOffset++] = bottomLeft;
            indexData[indexOffset++] = bottomRight;
        }
    }

    MeshData meshData;
    meshData.vertexData = vertexData;
    meshData.indexData = indexData;
    meshData.floatCount = floatCount;
    meshData.indexCount = indexCount;
    return meshData;
}

void destroyMeshData(MeshData &meshData)
{
    delete[] meshData.vertexData;
    delete[] meshData.indexData;
    meshData.vertexData = 0;
    meshData.indexData = 0;
    meshData.floatCount = 0;
    meshData.indexCount = 0;
}

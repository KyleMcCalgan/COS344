#include "Geometry.h"

#include <cmath>
#include <set>
#include <utility>

namespace
{
const int FLOATS_PER_VERTEX = 8;
const float PI = 3.14159265359f;

unsigned int *buildEdgeIndexData(const unsigned int *triangleIndices, int triangleIndexCount,
                                 int &edgeIndexCount)
{
    std::set<std::pair<unsigned int, unsigned int> > edges;

    for (int i = 0; i + 2 < triangleIndexCount; i += 3)
    {
        const unsigned int a = triangleIndices[i];
        const unsigned int b = triangleIndices[i + 1];
        const unsigned int c = triangleIndices[i + 2];

        const std::pair<unsigned int, unsigned int> ab =
            (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
        const std::pair<unsigned int, unsigned int> bc =
            (b < c) ? std::make_pair(b, c) : std::make_pair(c, b);
        const std::pair<unsigned int, unsigned int> ac =
            (a < c) ? std::make_pair(a, c) : std::make_pair(c, a);

        edges.insert(ab);
        edges.insert(bc);
        edges.insert(ac);
    }

    edgeIndexCount = static_cast<int>(edges.size()) * 2;
    unsigned int *edgeIndexData = new unsigned int[edgeIndexCount];
    int edgeOffset = 0;

    for (std::set<std::pair<unsigned int, unsigned int> >::const_iterator it = edges.begin();
         it != edges.end(); ++it)
    {
        edgeIndexData[edgeOffset++] = it->first;
        edgeIndexData[edgeOffset++] = it->second;
    }

    return edgeIndexData;
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
                indexData[indexOffset++] = bottomRight;
                indexData[indexOffset++] = bottomLeft;
                continue;
            }

            if (stack == stacks - 1)
            {
                indexData[indexOffset++] = topLeft;
                indexData[indexOffset++] = topRight;
                indexData[indexOffset++] = bottomLeft;
                continue;
            }

            indexData[indexOffset++] = topLeft;
            indexData[indexOffset++] = topRight;
            indexData[indexOffset++] = bottomLeft;

            indexData[indexOffset++] = topRight;
            indexData[indexOffset++] = bottomRight;
            indexData[indexOffset++] = bottomLeft;
        }
    }

    MeshData meshData;
    meshData.vertexData = vertexData;
    meshData.indexData = indexData;
    meshData.edgeIndexData = buildEdgeIndexData(indexData, indexCount, meshData.edgeIndexCount);
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
    meshData.edgeIndexData = buildEdgeIndexData(indexData, indexCount, meshData.edgeIndexCount);
    meshData.floatCount = floatCount;
    meshData.indexCount = indexCount;
    return meshData;
}

void destroyMeshData(MeshData &meshData)
{
    delete[] meshData.vertexData;
    delete[] meshData.indexData;
    delete[] meshData.edgeIndexData;
    meshData.vertexData = 0;
    meshData.indexData = 0;
    meshData.edgeIndexData = 0;
    meshData.floatCount = 0;
    meshData.indexCount = 0;
    meshData.edgeIndexCount = 0;
}
